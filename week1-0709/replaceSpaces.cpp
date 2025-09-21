#include <iostream>
#include <string>

// replace spaces with %20
void replaceSpaces(std::string &str)
{
    int oldSize = static_cast<int> (str.size());
    if (oldSize == 0)
        return;

    int spaceAmount = 0;
    for (char& c : str) {
        if (c == ' ')
            ++spaceAmount;
    }

    if (spaceAmount == 0)
        return;

    int newSize = oldSize + spaceAmount * 2;
    str.resize(newSize);

    int j = newSize - 1;
    for (int i = oldSize - 1; i >= 0; --i)
    {
        if (str[i] == ' ')
        {
            str[j] = '%';
            str[j - 1] = '0';
            str[j - 2] = '2';
            j -= 3;
        }
        else
        {
            str[j] = str[i];
            --j;
        }
    }
}

int main()
{
    std::string str = "This is a test   string";
    replaceSpaces(str);
    std::cout << str << std::endl;
}
