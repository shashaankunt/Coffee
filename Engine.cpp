#include "Engine.h"
#include "Main.h"

namespace iProlog
{

	Engine::Engine(const std::wstring &fname) : clauses(dload(fname)), cls(toNums(clauses)), syms(new LinkedHashMap<std::wstring, int>()), slist(std::vector<std::wstring>()), trail(new IntStack()), ustack(new IntStack()), imaps(index(clauses, vmaps)), vmaps(vcreate(MAXIND))
	{

	  makeHeap();




	  query = init();

	}

int Engine::MINSIZE = 1 << 15;

	int Engine::tag(int const t, int const w)
	{
	  return -((w << 3) + t);
	}

	int Engine::detag(int const w)
	{
	  return -w >> 3;
	}

	int Engine::tagOf(int const w)
	{
	  return -w & 7;
	}

	int Engine::addSym(const std::wstring &sym)
	{
	  boost::optional<int> I = syms->get(sym);
	  if (nullptr == I)
	  {
		constexpr int i = syms->size();
		I = boost::optional<int>(i);
		syms->put(sym, I);
		slist.push_back(sym);
	  }
	  return I.value();
	}

	std::wstring Engine::getSym(int const w)
	{
	  if (w < 0 || w >= slist.size())
	  {
		return L"BADSYMREF=" + std::to_wstring(w);
	  }
	  return slist[w];
	}

	void Engine::makeHeap()
	{
	  makeHeap(MINSIZE);
	}

	void Engine::makeHeap(int const size)
	{
	  heap = std::vector<int>(size);
	  clear();
	}

	int Engine::getTop()
	{
	  return top;
	}

	int Engine::setTop(int const top)
	{
	  return this->top = top;
	}

	void Engine::clear()
	{
	  //for (int i = 0; i <= top; i++)
	  //heap[i] = 0;
	  top = -1;
	}

	void Engine::push(int const i)
	{
	  heap[++top] = i;
	}

	int Engine::size()
	{
	  return top + 1;
	}

	void Engine::expand()
	{
	  constexpr int l = heap.size();
	  const std::vector<int> newstack = std::vector<int>(l << 1);

	  System::arraycopy(heap, 0, newstack, 0, l);
	  heap = newstack;
	}

	void Engine::ensureSize(int const more)
	{
	  if (1 + top + more >= heap.size())
	  {
		expand();
	  }
	}

	std::vector<std::vector<std::wstring>> Engine::maybeExpand(std::vector<std::wstring> &Ws)
	{
	  const std::wstring W = Ws[0];
	  if (W.length() < 2 || L"l:" != W.substr(0, 2))
	  {
		return nullptr;
	  }

	  constexpr int l = Ws.size();
	  const std::vector<std::vector<std::wstring>> Rss = std::vector<std::vector<std::wstring>>();
	  const std::wstring V = W.substr(2);
	  for (int i = 1; i < l; i++)
	  {
		const std::vector<std::wstring> Rs = std::vector<std::wstring>(4);
		const std::wstring Vi = 1 == i ? V : V + L"__" + std::to_wstring(i - 1);
		const std::wstring Vii = V + L"__" + std::to_wstring(i);
		Rs[0] = L"h:" + Vi;
		Rs[1] = L"c:list";
		Rs[2] = Ws[i];
		Rs[3] = i == l - 1 ? L"c:nil" : L"v:" + Vii;
		Rss.push_back(Rs);
	  }
	  return Rss;

	}

	std::vector<std::vector<std::wstring>> Engine::mapExpand(std::vector<std::vector<std::wstring>> &Wss)
	{
	  const std::vector<std::vector<std::wstring>> Rss = std::vector<std::vector<std::wstring>>();
	  for (auto Ws : Wss)
	  {

		const std::vector<std::vector<std::wstring>> Hss = maybeExpand(Ws);

		if (nullptr == Hss)
		{
		  const std::vector<std::wstring> ws = std::vector<std::wstring>(Ws.size());
		  for (int i = 0; i < ws.size(); i++)
		  {
			ws[i] = Ws.get(i);
		  }
		  Rss.push_back(ws);
		}
		else
		{
		  for (auto X : Hss)
		  {
			Rss.push_back(X);
		  }
		}
	  }
	  return Rss;
	}

	std::vector<Clause*> Engine::dload(const std::wstring &s)
	{
	  constexpr bool fromFile = true;
	  const std::vector<std::vector<std::vector<std::wstring>>> Wsss = Toks::toSentences(s, fromFile);

	  const std::vector<Clause*> Cs = std::vector<Clause*>();

	  for (auto Wss : Wsss)
	  {
		// clause starts here

		LinkedHashMap<std::wstring, IntStack*> * const refs = new LinkedHashMap<std::wstring, IntStack*>();
		IntStack * const cs = new IntStack();
		IntStack * const gs = new IntStack();

		const std::vector<std::vector<std::wstring>> Rss = mapExpand(Wss);
		int k = 0;
		for (auto ws : Rss)
		{

		  // head or body element starts here

		  constexpr int l = ws.size();
		  gs->push(tag(R, k++));
		  cs->push(tag(A, l));

		  for (auto w : ws)
		  {

			// head or body subterm starts here

			if (1 == w.length())
			{
			  w = L"c:" + w;
			}

			const std::wstring L = w.substr(2);

			switch (w.charAt(0))
			{
			  case L'c':
				cs->push(encode(C, L));
				k++;
			  break;
			  case L'n':
				cs->push(encode(N, L));
				k++;
			  break;
			  case L'v':
			  {
				IntStack *Is = refs->get(L);
				if (nullptr == Is)
				{
				  Is = new IntStack();
				  refs->put(L, Is);
				}
				Is->push(k);
				cs->push(tag(BAD, k)); // just in case we miss this
				k++;
			  }
			  break;
			  case L'h':
			  {
				IntStack *Is = refs->get(L);
				if (nullptr == Is)
				{
				  Is = new IntStack();
				  refs->put(L, Is);
				}
				Is->push(k - 1);
				cs->set(k - 1, tag(A, l - 1));
				gs->pop();
			  }
			  break;
			  default:
				Main::pp(L"FORGOTTEN=" + w);
			} // end subterm
		  } // end element
		} // end clause

		// linker
		const Iterator<IntStack*> K = refs->values().begin();

		while (K->hasNext())
		{
		  IntStack * const Is = K->next();

		  // finding the A among refs
		  int leader = -1;
		  for (int j : Is->toArray())
		  {
			if (A == tagOf(cs->get(j)))
			{
			  leader = j;

			  break;
			}
		  }
		  if (-1 == leader)
		  {
			// for vars, first V others U
			leader = Is->get(0);
			for (int i : Is->toArray())
			{
			  if (i == leader)
			  {
				cs->set(i, tag(V, i));
			  }
			  else
			  {
				cs->set(i, tag(U, leader));
			  }

			}
		  }
		  else
		  {
			for (int i : Is->toArray())
			{
			  if (i == leader)
			  {
				continue;
			  }
			  cs->set(i, tag(R, leader));
			}
		  }
			K++;
		}

		constexpr int neck = 1 == gs->size() ? cs->size() : detag(gs->get(1));
		const std::vector<int> tgs = gs->toArray();

		Clause * const C = putClause(cs->toArray(), tgs, neck);

		Cs.push_back(C);

	  } // end clause set

	  constexpr int ccount = Cs.size();
	  std::vector<Clause*> cls(ccount);
	  for (int i = 0; i < ccount; i++)
	  {
		cls[i] = Cs[i];
	  }
	  return cls;
	}

	std::vector<int> Engine::toNums(std::vector<Clause*> &clauses)
	{
	  constexpr int l = clauses.size();
	  const std::vector<int> cls = std::vector<int>(l);
	  for (int i = 0; i < l; i++)
	  {
		cls[i] = i;
	  }
	  return cls;
	}

	int Engine::encode(int const t, const std::wstring &s)
	{
	  int w;
	  try
	  {
		w = std::stoi(s);
	  }
//JAVA TO C++ CONVERTER WARNING: 'final' catch parameters are not available in C++:
//ORIGINAL LINE: catch (final Exception e)
	  catch (const std::exception &e)
	  {
		if (C == t)
		{
		  w = addSym(s);
		}
		else
		{
		  //pp("bad in encode=" + t + ":" + s);
		  return tag(BAD, 666);
		}
	  }
	  return tag(t, w);
	}

	bool Engine::isVAR(int const x)
	{
	  //final int t = tagOf(x);
	  //return V == t || U == t;
	  return tagOf(x) < 2;
	}

	int Engine::getRef(int const x)
	{
	  return heap[detag(x)];
	}

	void Engine::setRef(int const w, int const r)
	{
	  heap[detag(w)] = r;
	}

	void Engine::unwindTrail(int const savedTop)
	{
	  while (savedTop < trail->getTop())
	  {
		constexpr int href = trail->pop();
		// assert href is var

		setRef(href, href);
	  }
	}

	int Engine::deref(int x)
	{
	  while (isVAR(x))
	  {
		constexpr int r = getRef(x);
		if (r == x)
		{
		  break;
		}
		x = r;
	  }
	  return x;
	}

	std::wstring Engine::showTerm(int const x)
	{
	  return showTerm(exportTerm(x));
	}

	std::wstring Engine::showTerm(void *const O)
	{
	  if (dynamic_cast<std::vector<void*>>(O) != nullptr)
	  {
		return Arrays::deepToString(static_cast<std::vector<void*>>(O));
	  }
//JAVA TO C++ CONVERTER TODO TASK: There is no native C++ equivalent to 'toString':
	  return O->toString();
	}

	void Engine::ppTrail()
	{
	  for (int i = 0; i <= trail->getTop(); i++)
	  {
		constexpr int t = trail->get(i);
		Main::pp(L"trail[" + std::to_wstring(i) + L"]=" + showCell(t) + L":" + showTerm(t));
	  }
	}

	void *Engine::exportTerm(int x)
	{
	  x = deref(x);

	  constexpr int t = tagOf(x);
	  constexpr int w = detag(x);

	  auto res = nullptr;
	  switch (t)
	  {
		case C:
		  res = getSym(w);
		break;
		case N:
		  res = boost::optional<int>(w);
		break;
		case V:
		  //case U:
		  res = L"V" + std::to_wstring(w);
		break;
		case R:
		{

		  constexpr int a = heap[w];
		  if (A != tagOf(a))
		  {
			return L"*** should be A, found=" + showCell(a);
		  }
		  constexpr int n = detag(a);
		  std::vector<void*> arr(n);
		  constexpr int k = w + 1;
		  for (int i = 0; i < n; i++)
		  {
			constexpr int j = k + i;
			arr[i] = exportTerm(heap[j]);
		  }
		  res = arr;
		}
		break;
		default:
		  res = L"*BAD TERM*" + showCell(x);
	  }
	  return res;
	}

	std::vector<int> Engine::getSpine(std::vector<int> &cs)
	{
	  constexpr int a = cs[1];
	  constexpr int w = detag(a);
	  const std::vector<int> rs = std::vector<int>(w - 1);
	  for (int i = 0; i < w - 1; i++)
	  {
		constexpr int x = cs[3 + i];
		constexpr int t = tagOf(x);
		if (R != t)
		{
		  Main::pp(L"*** getSpine: unexpected tag=" + std::to_wstring(t));
		  return nullptr;
		}
		rs[i] = detag(x);
	  }
	  return rs;
	}

	std::wstring Engine::showCell(int const w)
	{
	  constexpr int t = tagOf(w);
	  constexpr int val = detag(w);
	  std::wstring s = L"";
	  switch (t)
	  {
		case V:
		  s = L"v:" + std::to_wstring(val);
		break;
		case U:
		  s = L"u:" + std::to_wstring(val);
		break;
		case N:
		  s = L"n:" + std::to_wstring(val);
		break;
		case C:
		  s = L"c:" + getSym(val);
		break;
		case R:
		  s = L"r:" + std::to_wstring(val);
		break;
		case A:
		  s = L"a:" + std::to_wstring(val);
		break;
		default:
		  s = L"*BAD*=" + std::to_wstring(w);
	  }
	  return s;
	}

	std::wstring Engine::showCells(int const base, int const len)
	{
	  StringBuilder * const buf = new StringBuilder();
	  for (int k = 0; k < len; k++)
	  {
		constexpr int instr = heap[base + k];

		buf->append(L"[" + std::to_wstring(base + k) + L"]");
		buf->append(showCell(instr));
		buf->append(L" ");
	  }
	  return buf->toString();
	}

	std::wstring Engine::showCells(std::vector<int> &cs)
	{
	  StringBuilder * const buf = new StringBuilder();
	  for (int k = 0; k < cs.size(); k++)
	  {
		buf->append(L"[" + std::to_wstring(k) + L"]");
		buf->append(showCell(cs[k]));
		buf->append(L" ");
	  }
	  return buf->toString();
	}

	void Engine::ppc(Spine *const C)
	{
	  // override
	}

	void Engine::ppGoals(IntList *const gs)
	{
	  // override
	}

	void Engine::ppSpines()
	{
	  // override
	}

	bool Engine::unify(int const base)
	{
	  while (!ustack->isEmpty())
	  {
		constexpr int x1 = deref(ustack->pop());
		constexpr int x2 = deref(ustack->pop());
		if (x1 != x2)
		{
		  constexpr int t1 = tagOf(x1);
		  constexpr int t2 = tagOf(x2);
		  constexpr int w1 = detag(x1);
		  constexpr int w2 = detag(x2);

		  if (isVAR(x1))
		  { // unb. var. v1
			if (isVAR(x2) && w2 > w1)
			{ // unb. var. v2
			  heap[w2] = x1;
			  if (w2 <= base)
			  {
				trail->push(x2);
			  }
			}
			else
			{ // x2 nonvar or older
			  heap[w1] = x2;
			  if (w1 <= base)
			  {
				trail->push(x1);
			  }
			}
		  }
		  else if (isVAR(x2))
		  { // x1 is NONVAR
			heap[w2] = x1;
			if (w2 <= base)
			{
			  trail->push(x2);
			}
		  }
		  else if (R == t1 && R == t2)
		  { // both should be R
			if (!unify_args(w1, w2))
			{
			  return false;
			}
		  }
		  else
		  {
			return false;
		  }
		}
	  }
	  return true;
	}

	bool Engine::unify_args(int const w1, int const w2)
	{
	  constexpr int v1 = heap[w1];
	  constexpr int v2 = heap[w2];
	  // both should be A
	  constexpr int n1 = detag(v1);
	  constexpr int n2 = detag(v2);
	  if (n1 != n2)
	  {
		return false;
	  }
	  constexpr int b1 = 1 + w1;
	  constexpr int b2 = 1 + w2;
	  for (int i = n1 - 1; i >= 0; i--)
	  {
		constexpr int i1 = b1 + i;
		constexpr int i2 = b2 + i;
		constexpr int u1 = heap[i1];
		constexpr int u2 = heap[i2];
		if (u1 == u2)
		{
		  continue;
		}
		ustack->push(u2);
		ustack->push(u1);
	  }
	  return true;
	}

	Clause *Engine::putClause(std::vector<int> &cs, std::vector<int> &gs, int const neck)
	{
	  constexpr int base = size();
	  constexpr int b = tag(V, base);
	  constexpr int len = cs.size();
	  pushCells(b, 0, len, cs);
	  for (int i = 0; i < gs.size(); i++)
	  {
		gs[i] = relocate(b, gs[i]);
	  }
	  const std::vector<int> xs = getIndexables(gs[0]);
	  return new Clause(len, gs, base, neck, xs);
	}

	int Engine::relocate(int const b, int const cell)
	{
	  return tagOf(cell) < 3 ? cell + b : cell;
	}

	void Engine::pushCells(int const b, int const from, int const to, int const base)
	{
	  ensureSize(to - from);
	  for (int i = from; i < to; i++)
	  {
		push(relocate(b, heap[base + i]));
	  }
	}

	void Engine::pushCells(int const b, int const from, int const to, std::vector<int> &cs)
	{
	  ensureSize(to - from);
	  for (int i = from; i < to; i++)
	  {
		push(relocate(b, cs[i]));
	  }
	}

	int Engine::pushHead(int const b, Clause *const C)
	{
	  pushCells(b, 0, C->neck, C->base);
	  constexpr int head = C->hgs[0];
	  return relocate(b, head);
	}

	std::vector<int> Engine::pushBody(int const b, int const head, Clause *const C)
	{
	  pushCells(b, C->neck, C->len, C->base);
	  constexpr int l = C->hgs->length;
	  const std::vector<int> gs = std::vector<int>(l);
	  gs[0] = head;
	  for (int k = 1; k < l; k++)
	  {
		constexpr int cell = C->hgs[k];
		gs[k] = relocate(b, cell);
	  }
	  return gs;
	}

	void Engine::makeIndexArgs(Spine *const G, int const goal)
	{
	  if (nullptr != G->xs)
	  {
		return;
	  }

	  constexpr int p = 1 + detag(goal);
	  constexpr int n = std::min(MAXIND, detag(getRef(goal)));

	  const std::vector<int> xs = std::vector<int>(MAXIND);

	  for (int i = 0; i < n; i++)
	  {
		constexpr int cell = deref(heap[p + i]);
		xs[i] = cell2index(cell);
	  }

	  G->xs = xs;

	  if (nullptr == imaps)
	  {
		return;
	  }
	  const std::vector<int> cs = IMap->get(imaps, vmaps, xs);
	  G->cs = cs;
	}

	std::vector<int> Engine::getIndexables(int const ref)
	{
	  constexpr int p = 1 + detag(ref);
	  constexpr int n = detag(getRef(ref));
	  const std::vector<int> xs = std::vector<int>(MAXIND);
	  for (int i = 0; i < MAXIND && i < n; i++)
	  {
		constexpr int cell = deref(heap[p + i]);
		xs[i] = cell2index(cell);
	  }
	  return xs;
	}

	int Engine::cell2index(int const cell)
	{
	  int x = 0;
	  constexpr int t = tagOf(cell);
	  switch (t)
	  {
		case R:
		  x = getRef(cell);
		break;
		case C:
		case N:
		  x = cell;
		break;
		// 0 otherwise - assert: tagging with R,C,N <>0
	  }
	  return x;
	}

	bool Engine::match(std::vector<int> &xs, Clause *const C0)
	{
	  for (int i = 0; i < MAXIND; i++)
	  {
		constexpr int x = xs[i];
		constexpr int y = C0->xs[i];
		if (0 == x || 0 == y)
		{
		  continue;
		}
		if (x != y)
		{
		  return false;
		}
	  }
	  return true;
	}

	Spine *Engine::unfold(Spine *const G)
	{

	  constexpr int ttop = trail->getTop();
	  constexpr int htop = getTop();
	  constexpr int base = htop + 1;

	  constexpr int goal = IntList::head(G->gs);

	  makeIndexArgs(G, goal);

	  constexpr int last = G->cs->length;
	  for (int k = G->k; k < last; k++)
	  {
		Clause * const C0 = clauses[G->cs[k]];

		if (!match(G->xs, C0))
		{
		  continue;
		}

		constexpr int base0 = base - C0->base;
		constexpr int b = tag(V, base0);
		constexpr int head = pushHead(b, C0);

		ustack->clear(); // set up unification stack

		ustack->push(head);
		ustack->push(goal);

		if (!unify(base))
		{
		  unwindTrail(ttop);
		  setTop(htop);
		  continue;
		}
		const std::vector<int> gs = pushBody(b, head, C0);
		IntList * const newgs = IntList::tail(IntList::app(gs, IntList::tail(G->gs)));
		G->k = k + 1;
		if (!IntList->isEmpty(newgs))
		{
		  return new Spine(gs, base, IntList::tail(G->gs), ttop, 0, cls);
		}
		else
		{
		  return answer(ttop);
		}
	  } // end for
	  return nullptr;
	}

	Clause *Engine::getQuery()
	{
	  return clauses[clauses.size() - 1];
	}

	Spine *Engine::init()
	{
	  constexpr int base = size();

	  Clause * const G = getQuery();
	  Spine * const Q = new Spine(G->hgs, base, IntList::empty, trail->getTop(), 0, cls);
	  spines->push(Q);
	  return Q;
	}

	Spine *Engine::answer(int const ttop)
	{
	  return new Spine(spines->get(0).hd, ttop);
	}

	bool Engine::hasClauses(Spine *const S)
	{
	  return S->k < S->cs->length;
	}

	bool Engine::hasGoals(Spine *const S)
	{
	  return !IntList->isEmpty(S->gs);
	}

	void Engine::popSpine()
	{
	  Spine * const G = spines->pop();
	  unwindTrail(G->ttop);
	  setTop(G->base - 1);
	}

	Spine *Engine::yield()
	{
	  while (!spines->isEmpty())
	  {
		Spine * const G = spines->peek();
		if (!hasClauses(G))
		{
		  popSpine(); // no clauses left
		  continue;
		}
		Spine * const C = unfold(G);
		if (nullptr == C)
		{
		  popSpine(); // no matches
		  continue;
		}
		if (hasGoals(C))
		{
		  spines->push(C);
		  continue;
		}
		return C; // answer
	  }
	  return nullptr;
	}

	void *Engine::ask()
	{
	  query = yield();
	  if (nullptr == query)
	  {
		return nullptr;
	  }
	  constexpr int res = answer(query->ttop)->hd;
	  constexpr auto R = exportTerm(res);
	  unwindTrail(query->ttop);
	  return R;
	}

	void Engine::run()
	{
	  long long ctr = 0LL;
	  for (;; ctr++)
	  {
		constexpr auto A = ask();
		if (nullptr == A)
		{
		  break;
		}
		if (ctr < 5)
		{
			Prog::println(L"[" + std::to_wstring(ctr) + L"] " + L"*** ANSWER=" + showTerm(A));
		}
	  }
	  if (ctr > 5)
	  {
		  Prog::println(L"...");
	  }
	  Prog::println(L"TOTAL ANSWERS=" + std::to_wstring(ctr));
	}

	std::vector<IntMap*> Engine::vcreate(int const l)
	{
	  std::vector<IntMap*> vss(l);
	  for (int i = 0; i < l; i++)
	  {
		vss[i] = new IntMap();
	  }
	  return vss;
	}

	void Engine::put(std::vector<IMap<int>*> &imaps, std::vector<IntMap*> &vss, std::vector<int> &keys, int const val)
	{
	  for (int i = 0; i < imaps.size(); i++)
	  {
		constexpr int key = keys[i];
		if (key != 0)
		{
		  IMap::put(imaps, i, key, val);
		}
		else
		{
		  vss[i]->add(val);
		}
	  }
	}

	std::vector<IMap<int>*> Engine::index(std::vector<Clause*> &clauses, std::vector<IntMap*> &vmaps)
	{
	  if (clauses.size() < START_INDEX)
	  {
		return nullptr;
	  }

	  std::vector<IMap<int>*> imaps = IMap::create(vmaps.size());
	  for (int i = 0; i < clauses.size(); i++)
	  {
		Clause * const c = clauses[i];

		put(imaps, vmaps, c->xs, i + 1); // $$$ UGLY INC

	  }
	  Main::pp(L"INDEX");
	  Main::pp(IMap::show(imaps));

	  Main::pp(Arrays->toString(vmaps));
	  Main::pp(L"");
	  return imaps;
	}
}
