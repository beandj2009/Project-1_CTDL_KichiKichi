#include <iostream>
#include "Admin.h"
#include "User.h"
//check xem có phải số không
bool isNumber(const string& s)
{
    if (s.empty()) return false;
    for (char c : s)
        if (!isdigit((unsigned char)c))
            return false;
    return true;
}

void welcomeScreen()
{
    string strInput;
    int iChoice = -1;

    do
    {
        system("cls");
        Admin::setColor(9);
        cout << "\n\t\t============================================\n";
        cout << "\t\t||";
        Admin::setColor(12);
        cout << "             KichiKichi's Bank          ";
        Admin::setColor(9);
        cout << "||\n";
        cout << "\t\t============================================\n";

        Admin::setColor(9);
        cout << "\t\t||  ";
        Admin::setColor(7);
        cout << "1. Dang Nhap Admin                    ";
        Admin::setColor(9);
        cout << "||\n";

        cout << "\t\t||  ";
        Admin::setColor(7);
        cout << "2. Dang Nhap User                     ";
        Admin::setColor(9);
        cout << "||\n";

        cout << "\t\t||  ";
        Admin::setColor(7);
        cout << "0. Thoat                              ";
        Admin::setColor(9);
        cout << "||\n";

        Admin::setColor(9);
        cout << "\t\t============================================\n";

        Admin::setColor(14);
        cout << "\n\t\tNhap lua chon: ";

        Admin::setColor(7);
        cin >> strInput;

        if(!isNumber(strInput))
        {
            Admin::setColor(4);
            cout << "\n\t\tChi duoc nhap so (0-2)!";
            Sleep(1200);
            continue;
        }

        iChoice = stoi(strInput);

        switch(iChoice)
        {
            case 1:
                Admin::dangNhapUI("Admin.txt");
                break;
            case 2:
                User::dangNhapUI("TheTu.txt");
                break;
            case 0:
                Admin::setColor(2);
                cout <<"\n\t\tHen Gap Lai Quy Khach!";
                Sleep(1000);
                Admin::setColor(7);
                break;
            default:
                Admin::setColor(4);
                cout << "\n\t\tLua chon khong hop le!";
                Sleep(1000);
                break;
        }

    } while(iChoice != 0);
}

int main()
{
    welcomeScreen();

    return 0;

}
