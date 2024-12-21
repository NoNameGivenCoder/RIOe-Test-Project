#include <cstring>
#include <string>
#include <unordered_map>

#include "misc/rio_MemUtil.h"

namespace rioe::Message
{
	class Dictionary
	{
	public:
		Dictionary() : mKeyCount(0), mKeys(nullptr), mValues(nullptr) {};
		~Dictionary() { Clear(); };

		// Adds an entry to the dictionary using a key and value
		void AddEntry(const char* key, const char* value);
		// Gets a value based on the key provided
		const char* GetValue(const char* key);
	private:
		void Clear()
		{
			for (size_t i = 0; i < mKeyCount; i++)
			{
				free(mKeys[i]);
				free(mValues[i]);
			}

			free(mKeys);
			free(mValues);
		}

		size_t mKeyCount;
		char** mKeys;
		char** mValues;
	};
}