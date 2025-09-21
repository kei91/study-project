#include <iostream>
#include <string>

// Convert string from aaaaaabbbbccccccc to a6b4c7, abc will remane the same
//
void CompressStr(std::string& str)
{
    int oldSize = static_cast<int>(str.size());
    if (oldSize == 0)
        return;

    int repeatAmount = 0;
    int j = 0;
    for (int i = 0; i < oldSize; ++i)
    {
        if (i == oldSize - 1 || str[i] != str[i + 1])
        {
            str[j] = str[i];

            int numSize = 0;
            if (repeatAmount != 0) {
                ++repeatAmount;
                std::string num = std::to_string(repeatAmount);
                numSize = static_cast<int>(num.size());
                for (int k = 0; k < numSize; ++k) {
                    str[j + k + 1] = num[k];
                }
            }

            j += numSize + 1;

            repeatAmount = 0;
        }
        else
        {
            ++repeatAmount;
        }
    }

    str.resize(j);
}

int main()
{
    std::string str = "aaaaaabbbbccccccc";
    CompressStr(str);
    std::cout << str << std::endl;

    str = "abc";
    CompressStr(str);
    std::cout << str << std::endl;
    
    str = "aaaaaaaaaabc";
    CompressStr(str);
    std::cout << str << std::endl;

    str = "abccccccccccccccccccccc";
    CompressStr(str);
    std::cout << str << std::endl;
}
