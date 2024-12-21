#include "rio-e/Types/Message/Dictionary.h"

namespace rioe::Message
{
	void Dictionary::AddEntry(const char* key, const char* value)
	{
		mKeyCount++;

		mKeys = (char**)realloc(mKeys, mKeyCount * sizeof(char*));
		mValues = (char**)realloc(mValues, mKeyCount * sizeof(char*));

		mKeys[mKeyCount - 1] = strdup(key);
		mValues[mKeyCount - 1] = strdup(value);
	}

	const char* Dictionary::GetValue(const char* key)
	{
		for (size_t i = 0; i < mKeyCount; ++i) {
			if (strcmp(mKeys[i], key) == 0) {
				return mValues[i];
			}
		}
		return nullptr;
	}
}