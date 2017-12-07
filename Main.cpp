#include "Main.h"
#include "Engine.h"

namespace iProlog
{

	void Main::println(void *const o)
	{
	  std::wcout << o << std::endl;
	}

	void Main::pp(void *const o)
	{
	  std::wcout << o << std::endl;
	}

	void Main::run(const std::wstring &fname0)
	{
	  constexpr bool p = true;

	  const std::wstring fname = fname0 + L".nl";
	  Engine *P;

	  if (p)
	  {
		P = new Prog(fname);
		pp(L"CODE");
		(static_cast<Prog*>(P))->ppCode();
	  }
	  else
	  {
		P = new Engine(fname);
	  }

	  pp(L"RUNNING");
	  constexpr long long t1 = System::nanoTime();
	  P->run();
	  constexpr long long t2 = System::nanoTime();
	  std::wcout << L"time=" << (t2 - t1) / 1000000000.0 << std::endl;

	}

	void Main::srun(const std::wstring &fname0)
	{
	  const std::wstring fname = fname0 + L".nl";
	  Prog * const P = new Prog(fname);

	  pp(L"CODE");
	  P->ppCode();

	  pp(L"RUNNING");
	  constexpr long long t1 = System::nanoTime();

	  Stream<void*> * const S = P->stream();
	  S->forEach([&] (void *x)
	  {
	  Main::pp(P->showTerm(x));
	  });

	  constexpr long long t2 = System::nanoTime();
	  std::wcout << L"time=" << (t2 - t1) / 1000000000.0 << std::endl;
	}

	void Main::main(std::vector<std::wstring> &args)
	{
		  std::wstring fname = args[0];
	  run(fname);
	}
}
