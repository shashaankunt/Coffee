#include "IntMap.h"

	IntMap::IntMap() : IntMap(1 << 2)
	{
	}

	IntMap::IntMap(int const size) : IntMap(size, 0.75f)
	{
	}

	IntMap::IntMap(int const size, float const fillFactor) : m_fillFactor(fillFactor)
	{
	  if (fillFactor <= 0 || fillFactor >= 1)
	  {
		throw std::invalid_argument("FillFactor must be in (0, 1)");
	  }
	  if (size <= 0)
	  {
		throw std::invalid_argument("Size must be positive!");
	  }
	  constexpr int capacity = arraySize(size, fillFactor);
	  m_mask = capacity - 1;
	  m_mask2 = capacity * 2 - 1;

	  m_data = std::vector<int>(capacity * 2);
	  m_threshold = static_cast<int>(capacity * fillFactor);
	}

	int IntMap::get(int const key)
	{
	  int ptr = (phiMix(key) & m_mask) << 1;

	  if (key == FREE_KEY)
	  {
		return m_hasFreeKey ? m_freeValue : NO_VALUE;
	  }

	  int k = m_data[ptr];

	  if (k == FREE_KEY)
	  {
		return NO_VALUE; //end of chain already
	  }
	  if (k == key) //we check FREE prior to this call
	  {
		return m_data[ptr + 1];
	  }

	  while (true)
	  {
		ptr = ptr + 2 & m_mask2; //that's next index
		k = m_data[ptr];
		if (k == FREE_KEY)
		{
		  return NO_VALUE;
		}
		if (k == key)
		{
		  return m_data[ptr + 1];
		}
	  }
	}

	bool IntMap::contains(int const key)
	{
	  return NO_VALUE != get(key);
	}

	bool IntMap::add(int const key)
	{
	  return NO_VALUE != put(key, 666);
	}

	bool IntMap::delete_Renamed(int const key)
	{
	  return NO_VALUE != remove(key);
	}

	bool IntMap::isEmpty()
	{
	  return 0 == m_size;
	}

	void IntMap::intersect0(IntMap *const m, std::vector<IntMap*> &maps, std::vector<IntMap*> &vmaps, IntStack *const r)
	{
	  const std::vector<int> data = m->m_data;
	  for (int k = 0; k < data.size(); k += 2)
	  {
		bool found = true;
		constexpr int key = data[k];
		if (FREE_KEY == key)
		{
		  continue;
		}
		for (int i = 1; i < maps.size(); i++)
		{
		  IntMap * const map = maps[i];
		  constexpr int val = map->get(key);

		  if (NO_VALUE == val)
		  {
			IntMap * const vmap = vmaps[i];
			constexpr int vval = vmap->get(key);
			if (NO_VALUE == vval)
			{
			  found = false;
			  break;
			}
		  }
		}
		if (found)
		{
		  r->push(key);
		}
	  }
	}

	IntStack *IntMap::intersect(std::vector<IntMap*> &maps, std::vector<IntMap*> &vmaps)
	{
	  IntStack * const r = new IntStack();

	  intersect0(maps[0], maps, vmaps, r);
	  intersect0(vmaps[0], maps, vmaps, r);
	  return r;
	}

	int IntMap::put(int const key, int const value)
	{
	  if (key == FREE_KEY)
	  {
		constexpr int ret = m_freeValue;
		if (!m_hasFreeKey)
		{
		  ++m_size;
		}
		m_hasFreeKey = true;
		m_freeValue = value;
		return ret;
	  }

	  int ptr = (phiMix(key) & m_mask) << 1;
	  int k = m_data[ptr];
	  if (k == FREE_KEY) //end of chain already
	  {
		m_data[ptr] = key;
		m_data[ptr + 1] = value;
		if (m_size >= m_threshold)
		{
		  rehash(m_data.size() * 2); //size is set inside
		}
		else
		{
		  ++m_size;
		}
		return NO_VALUE;
	  }
	  else if (k == key) //we check FREE prior to this call
	  {
		constexpr int ret = m_data[ptr + 1];
		m_data[ptr + 1] = value;
		return ret;
	  }

	  while (true)
	  {
		ptr = ptr + 2 & m_mask2; //that's next index calculation
		k = m_data[ptr];
		if (k == FREE_KEY)
		{
		  m_data[ptr] = key;
		  m_data[ptr + 1] = value;
		  if (m_size >= m_threshold)
		  {
			rehash(m_data.size() * 2); //size is set inside
		  }
		  else
		  {
			++m_size;
		  }
		  return NO_VALUE;
		}
		else if (k == key)
		{
		  constexpr int ret = m_data[ptr + 1];
		  m_data[ptr + 1] = value;
		  return ret;
		}
	  }
	}

	int IntMap::remove(int const key)
	{
	  if (key == FREE_KEY)
	  {
		if (!m_hasFreeKey)
		{
		  return NO_VALUE;
		}
		m_hasFreeKey = false;
		--m_size;
		return m_freeValue; //value is not cleaned
	  }

	  int ptr = (phiMix(key) & m_mask) << 1;
	  int k = m_data[ptr];
	  if (k == key) //we check FREE prior to this call
	  {
		constexpr int res = m_data[ptr + 1];
		shiftKeys(ptr);
		--m_size;
		return res;
	  }
	  else if (k == FREE_KEY)
	  {
		return NO_VALUE; //end of chain already
	  }
	  while (true)
	  {
		ptr = ptr + 2 & m_mask2; //that's next index calculation
		k = m_data[ptr];
		if (k == key)
		{
		  constexpr int res = m_data[ptr + 1];
		  shiftKeys(ptr);
		  --m_size;
		  return res;
		}
		else if (k == FREE_KEY)
		{
		  return NO_VALUE;
		}
	  }
	}

	int IntMap::shiftKeys(int pos)
	{
	  // Shift entries with the same hash.
	  int last, slot;
	  int k;
	  const std::vector<int> data = m_data;
	  while (true)
	  {
		pos = (last = pos) + 2 & m_mask2;
		while (true)
		{
		  if ((k = data[pos]) == FREE_KEY)
		  {
			data[last] = FREE_KEY;
			return last;
		  }
		  slot = (phiMix(k) & m_mask) << 1; //calculate the starting slot for the current key
		  if (last <= pos ? last >= slot || slot > pos : last >= slot && slot > pos)
		  {
			break;
		  }
		  pos = pos + 2 & m_mask2; //go to the next entry
		}
		data[last] = k;
		data[last + 1] = data[pos + 1];
	  }
	}

	int IntMap::size()
	{
	  return m_size;
	}

	void IntMap::rehash(int const newCapacity)
	{
	  m_threshold = static_cast<int>(newCapacity / 2 * m_fillFactor);
	  m_mask = newCapacity / 2 - 1;
	  m_mask2 = newCapacity - 1;

	  constexpr int oldCapacity = m_data.size();
	  const std::vector<int> oldData = m_data;

	  m_data = std::vector<int>(newCapacity);
	  m_size = m_hasFreeKey ? 1 : 0;

	  for (int i = 0; i < oldCapacity; i += 2)
	  {
		constexpr int oldKey = oldData[i];
		if (oldKey != FREE_KEY)
		{
		  put(oldKey, oldData[i + 1]);
		}
	  }
	}

	long long IntMap::nextPowerOfTwo(long long x)
	{
	  if (x == 0)
	  {
		return 1;
	  }
	  x--;
	  x |= x >> 1;
	  x |= x >> 2;
	  x |= x >> 4;
	  x |= x >> 8;
	  x |= x >> 16;
	  return (x | x >> 32) + 1;
	}

	int IntMap::arraySize(int const expected, float const f)
	{
	  constexpr long long s = std::max(2, nextPowerOfTwo(static_cast<long long>(std::ceil(expected / f))));
	  if (s > 1 << 30)
	  {
		throw std::invalid_argument("Too large (" + std::to_wstring(expected) + L" expected elements with load factor " + std::to_wstring(f) + L")");
	  }
	  return static_cast<int>(s);
	}

	int IntMap::phiMix(int const x)
	{
	  constexpr int h = x * INT_PHI;
	  return h ^ h >> 16;
	}

	std::wstring IntMap::toString()
	{
	  //return java.util.Arrays.toString(m_data);
	  StringBuilder * const b = new StringBuilder(L"{");
	  constexpr int l = m_data.size();
	  bool first = true;
	  for (int i = 0; i < l; i += 2)
	  {

		constexpr int v = m_data[i];
		if (v != FREE_KEY)
		{
		  if (!first)
		  {
			b->append(L',');
		  }
		  first = false;
		  b->append(v - 1);
		}
	  }
	  b->append(L"}");
	  return b->toString();
	}
}
