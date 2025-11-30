#pragma once
#include "Admin.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <conio.h>
#include <cctype>
#include <ctime>
#include <cstdio>
#include <windows.h>

using namespace std;

// LinkedList<T> đã được định nghĩa trong Admin.h
template<typename T>
class LinkedList;

class User;

// Dùng lại LinkedList cho User
using UserList = LinkedList<User>;
using UserNode = LinkedList<User>::Node;

/*===========================================================
||                          USER                           ||
===========================================================*/
class User
{
    string     strID, strPin;
    long long  llSoDu;
    bool       bLocked;

public:
    //========= Constructor - Destructor ===========
    User(const string& _strID = " ",
         const string& _strPin = " ",
         long long _llSoDu = 50000,
         bool _bLocked = false)
        : strID(_strID), strPin(_strPin),
          llSoDu(_llSoDu), bLocked(_bLocked) {}

    User(const User& u)
        : strID(u.strID), strPin(u.strPin),
          llSoDu(u.llSoDu), bLocked(u.bLocked) {}

    //============= Getter - Setter ================
    string    getID()      const { return strID; }
    string    getPin()     const { return strPin; }
    long long getSoDu()    const { return llSoDu; }
    bool      getLocked()  const { return bLocked; }

    void setID   (const string& _strID)  { strID = _strID; }
    void setPin  (const string& _strPin) { strPin = _strPin; }
    void setSoDu (long long _llSoDu)     { llSoDu = _llSoDu; }
    void setLocked(bool bVal)           { bLocked = bVal; }

    /******** Một số hàm bổ trợ User *********/
    static void docDanhSachTheTu(const string& filePath, UserList &listOut);
    static void khoaTaiKhoan(const string& filePath, const string& strID);
    static void capNhatTrangThaiFileInfo(const string& strID, bool bLock);

    static void menuUser(const string& strID);
    static bool dangNhapUI(const string& filePath);

    /******** Chức năng chính của User *********/
    static void xemThongTinTaiKhoan(const string& strID);
    static void rutTien(const string& strID);
    static void chuyenTien(const string& strID);
    static void xemNoiDungGiaoDich(const string& strID);
    static void doiMaPin(const string& strID);
};


/******** Một số hàm bổ trợ User *********/

/*******************************************************
*@Description: Đọc danh sách thẻ/tài khoản User từ file.
*@Param:       filePath – đường dẫn tới FILE_THETU;
*              listOut  – danh sách liên kết cần nạp dữ liệu.
*@Return:      none
*@Attention:   Gọi clear() danh sách trước khi load dữ liệu mới.
********************************************************/
inline void User::docDanhSachTheTu(const string& filePath, UserList& listOut)
{
    listOut.clear();
    ifstream ifUser(filePath);

    if (!ifUser.is_open()) {
        cerr << "Khong the mo file " << filePath << "\n";
        return;
    }

    string strFileID, strFilePin;
    int    iStatus;

    while (ifUser >> strFileID >> strFilePin >> iStatus)
    {
        User u(strFileID, strFilePin);
        u.setLocked(iStatus == 0);  // 0 = KHOA, 1 = DANG HOAT DONG
        listOut.addTail(u);
    }

    ifUser.close();
}

/*******************************************************
*@Description: Khóa tài khoản User trong danh sách thẻ.
*@Param:       filePath – đường dẫn FILE_THETU;
*              strID    – ID tài khoản cần khóa.
*@Return:      none
*@Attention:   Ghi lại toàn bộ danh sách vào file sau khi khóa.
********************************************************/
inline void User::khoaTaiKhoan(const string& filePath, const string& strID)
{
    UserList list;
    docDanhSachTheTu(filePath, list);

    bool bFound = false;

    for (UserNode* p = list.pHead; p != nullptr; p = p->pNext)
    {
        if (p->data.getID() == strID)
        {
            p->data.setLocked(true);
            bFound = true;
            break;
        }
    }

    if (!bFound) return;

    ofstream ofOut(filePath);
    if (!ofOut.is_open())
    {
        cerr << "Khong the ghi file " << filePath << "\n";
        return;
    }

    for (UserNode* p = list.pHead; p != nullptr; p = p->pNext)
    {
        ofOut << p->data.getID() << " "
              << p->data.getPin() << " "
              << (p->data.getLocked() ? 0 : 1) << "\n";
    }
    ofOut.close();
}

/*******************************************************
*@Description: Cập nhật trạng thái (KHOA / ĐANG HOẠT ĐỘNG)
*              trong file thông tin [ID].txt.
*@Param:       strID – ID tài khoản;
*              bLock – true nếu khóa, false nếu mở khóa.
*@Return:      none
*@Attention:   Nếu không mở được file [ID].txt thì hàm return luôn.
********************************************************/
inline void User::capNhatTrangThaiFileInfo(const string& strID, bool bLock)
{
    string infoFile = strID + ".txt";
    ifstream fin(infoFile.c_str());
    if (!fin.is_open())
    {
        return;
    }

    vector<string> vLines;
    string strLine;

    while (getline(fin, strLine))
    {
        vLines.push_back(strLine);
    }
    fin.close();

    for (int i = 0; i < (int)vLines.size(); i++)
    {
        if (vLines[i].find("Trang thai:") != string::npos)
        {
            if (bLock)
                vLines[i] = "Trang thai: KHOA";
            else
                vLines[i] = "Trang thai: DANG HOAT DONG";
            break;
        }
    }

    ofstream fout(infoFile.c_str());
    if (!fout.is_open())
    {
        return;
    }

    for (int i = 0; i < (int)vLines.size(); i++)
    {
        fout << vLines[i] << "\n";
    }
    fout.close();
}


/*******************************************************
*@Description: Hiển thị menu User và xử lý lựa chọn.
*@Param:       strID – ID tài khoản đang đăng nhập.
*@Return:      none
*@Attention:   Gọi các chức năng xem/rút/chuyển/đổi PIN.
********************************************************/
inline void User::menuUser(const string& strID)
{
    int    iChoice = -1;
    string strInput;

    do
    {
        system("cls");
        Admin::setColor(9);
        cout << "\n\t\t============================================\n";
        cout << "\t\t||";
        Admin::setColor(12);
        cout << "               MENU USER                ";
        Admin::setColor(9);
        cout << "||\n";
        cout << "\t\t============================================\n";

        Admin::setColor(9);
        cout << "\t\t||  "; Admin::setColor(7);
        cout << "1. Xem thong tin tai khoan            ";
        Admin::setColor(9); cout << "||\n";

        cout << "\t\t||  "; Admin::setColor(7);
        cout << "2. Rut tien                           ";
        Admin::setColor(9); cout << "||\n";

        cout << "\t\t||  "; Admin::setColor(7);
        cout << "3. Chuyen tien                        ";
        Admin::setColor(9); cout << "||\n";

        cout << "\t\t||  "; Admin::setColor(7);
        cout << "4. Xem noi dung giao dich             ";
        Admin::setColor(9); cout << "||\n";

        cout << "\t\t||  "; Admin::setColor(7);
        cout << "5. Doi ma Pin                         ";
        Admin::setColor(9); cout << "||\n";

        cout << "\t\t||  "; Admin::setColor(7);
        cout << "6. Thoat                              ";
        Admin::setColor(9); cout << "||\n";

        cout << "\t\t============================================\n";

        Admin::setColor(14);
        cout << "\n\n\t\tHay nhap lua chon: ";
        Admin::setColor(7);
        cin >> strInput;

        if (!Admin::isNumber(strInput))
        {
            Admin::setColor(4);
            cout << "\n\t\tChi duoc nhap so (1-6)!";
            Sleep(1200);
            continue;
        }

        iChoice = stoi(strInput);

        switch(iChoice)
        {
        case 1: xemThongTinTaiKhoan(strID);   break;
        case 2: rutTien(strID);               break;
        case 3: chuyenTien(strID);            break;
        case 4: xemNoiDungGiaoDich(strID);    break;
        case 5: doiMaPin(strID);              break;
        case 6: break;
        default:
            Admin::setColor(12);
            cout << "\n\tLua chon khong hop le!\n";
            Admin::setColor(7);
            Sleep(1000);
            break;
        }
    } while (iChoice != 6);
}


/*******************************************************
*@Description: Giao diện đăng nhập User bằng ID + PIN.
*@Param:       filePath – đường dẫn tới FILE_THETU.
*@Return:      true nếu đăng nhập thành công, false nếu lỗi/khóa.
*@Attention:   Sai PIN 3 lần sẽ tự động khóa tài khoản và cập nhật file.
********************************************************/
inline bool User::dangNhapUI(const string& filePath)
{
    string strIDIn, strPinIn;

    // ================== NHAP ID, KIEM TRA TON TAI + TRANG THAI ==================
    while (true)
    {
        system("cls");

        Admin::setColor(9);
        cout << "\n\t\t============================================\n";
        cout << "\t\t||";
        Admin::setColor(12);
        cout << "               DANG NHAP USER           ";
        Admin::setColor(9);
        cout << "||\n";
        cout << "\t\t============================================\n";

        Admin::setColor(14);
        cout << "\t\t ID: ";
        Admin::setColor(7);
        cin >> strIDIn;

        UserList list;
        User::docDanhSachTheTu(filePath, list);

        bool bFound  = false;
        bool bLocked = false;

        for (UserNode* p = list.pHead; p != nullptr; p = p->pNext)
        {
            if (p->data.getID() == strIDIn)
            {
                bFound  = true;
                bLocked = p->data.getLocked();
                break;
            }
        }

        system("cls");

        if (!bFound)
        {
            Admin::setColor(12);
            cout << "\n\t\tTai khoan khong ton tai. Vui long nhap lai ID!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        if (bLocked)
        {
            Admin::setColor(4);
            cout << "\n\t\tTai khoan cua ban dang bi KHOA. Vui long lien he ADMIN!\n";
            Admin::setColor(7);
            system("pause");
            return false;
        }

        break;
    }


    system("cls");
    Admin::setColor(9);
    cout << "\n\t\t============================================\n";
    cout << "\t\t||";
    Admin::setColor(12);
    cout << "               DANG NHAP USER           ";
    Admin::setColor(9);
    cout << "||\n";
    cout << "\t\t============================================\n";

    Admin::setColor(14);
    cout << "\t\tID: ";
    Admin::setColor(7);
    cout << strIDIn << "\n\n";

    int    iErrorCount  = 0;
    string strIDCanKhoa = strIDIn;

    while (iErrorCount < 3)
    {
        Admin::setColor(14);
        cout << "\t\tPin: ";
        Admin::setColor(7);
        strPinIn = Admin::pinHidden();

        UserList list;
        User::docDanhSachTheTu(filePath, list);

        bool bPinDung    = false;
        bool bPinMacDinh = false;
        bool bLocked     = false;

        for (UserNode* p = list.pHead; p != nullptr; p = p->pNext)
        {
            if (p->data.getID() == strIDIn)
            {
                bLocked = p->data.getLocked();
                if (!bLocked && p->data.getPin() == strPinIn)
                {
                    bPinDung    = true;
                    bPinMacDinh = (strPinIn == "123456");
                }
                break;
            }
        }

        if (bLocked)
        {
            Admin::setColor(4);
            cout << "\n\n\t\tTai khoan cua ban dang bi KHOA. Vui long lien he ADMIN!\n";
            Admin::setColor(7);
            system("pause");
            return false;
        }


        if (!bPinDung)
        {
            ++iErrorCount;
            Admin::setColor(12);
            cout << "\n\t\tPIN khong chinh xac!\n";

            if (iErrorCount < 3)
            {
                Admin::setColor(2);
                cout << "\t\tBan con " << (3 - iErrorCount) << " lan de thu.\n\n";
                Admin::setColor(7);
                Sleep(1000);
                continue;
            }
            else
            {
                Admin::setColor(4);
                cout << "\t\tBan da nhap sai PIN 3 lan.\n";
                cout << "\t\tTai khoan cua ban da bi KHOA do nhap sai PIN 3 lan.\n";
                Admin::setColor(7);

                khoaTaiKhoan(filePath, strIDCanKhoa);
                capNhatTrangThaiFileInfo(strIDCanKhoa, true);

                Sleep(1500);
                return false;
            }
        }


        if (bPinMacDinh)
        {
            string strNewPin, strConfirm;
            while (true)
            {
                system("cls");
                Admin::setColor(9);
                cout << "\n\t\t============================================\n";
                cout << "\t\t||";
                Admin::setColor(12);
                cout << "      DOI PIN LAN DANG NHAP DAU TIEN    ";
                Admin::setColor(9);
                cout << "||\n";
                cout << "\t\t============================================\n\n";

                Admin::setColor(14);
                cout << "\t\tPIN hien tai dang la mac dinh (123456).\n";
                cout << "\t\tVui long nhap PIN moi (6 chu so, khac PIN cu).\n\n";

                cout << "\t\tNhap PIN moi: ";
                Admin::setColor(7);
                strNewPin = Admin::pinHidden();

                Admin::setColor(14);
                cout << "\t\tNhap lai PIN moi: ";
                Admin::setColor(7);
                strConfirm = Admin::pinHidden();

                if (strNewPin != strConfirm)
                {
                    Admin::setColor(12);
                    cout << "\n\t\tHai lan nhap PIN khong giong nhau!\n";
                    Admin::setColor(7);
                    Sleep(1000);
                    continue;
                }

                bool bAllDigit = !strNewPin.empty();
                for (int i = 0; i < (int)strNewPin.size(); ++i)
                    if (!isdigit((unsigned char)strNewPin[i]))
                        bAllDigit = false;

                if (!bAllDigit || strNewPin.length() != 6)
                {
                    Admin::setColor(12);
                    cout << "\n\t\tPIN moi phai GOM DUNG 6 CHU SO!\n";
                    Admin::setColor(7);
                    Sleep(1000);
                    continue;
                }

                if (strNewPin == strPinIn)
                {
                    Admin::setColor(12);
                    cout << "\n\t\tPIN moi khong duoc giong PIN cu!\n";
                    Admin::setColor(7);
                    Sleep(1000);
                    continue;
                }

                break;
            }

            // Cập nhật pin trong file TheTu.txt
            UserList list2;
            User::docDanhSachTheTu(filePath, list2);

            ofstream ofOut(filePath);
            if (ofOut.is_open())
            {
                for (UserNode* p = list2.pHead; p != nullptr; p = p->pNext)
                {
                    if (p->data.getID() == strIDIn)
                        ofOut << p->data.getID() << " " << strNewPin
                              << " " << (p->data.getLocked() ? 0 : 1) << "\n";
                    else
                        ofOut << p->data.getID() << " " << p->data.getPin()
                              << " " << (p->data.getLocked() ? 0 : 1) << "\n";
                }
                ofOut.close();
            }

            // Cập nhật PIN trong file [ID].txt
            string infoFile = strIDIn + ".txt";
            ifstream fin(infoFile.c_str());

            if (fin.is_open())
            {
                vector<string> vLines;
                string line;

                while (getline(fin, line))
                    vLines.push_back(line);

                fin.close();

                for (int i = 0; i < (int)vLines.size(); ++i)
                {
                    if (vLines[i].find("PIN") != string::npos)
                    {
                        vLines[i] = "PIN: " + strNewPin;
                        break;
                    }
                }

                ofstream fout(infoFile.c_str());
                if (fout.is_open())
                {
                    for (int i = 0; i < (int)vLines.size(); ++i)
                        fout << vLines[i] << "\n";
                    fout.close();
                }
            }

            Admin::setColor(10);
            cout << "\n\t\tDoi PIN thanh cong!\n";
            Admin::setColor(7);
            Sleep(800);
        }


        Admin::setColor(10);
        cout << "\n\t\tDang nhap thanh cong!\n";
        Admin::setColor(7);
        Sleep(800);
        menuUser(strIDIn);
        return true;
    }

    return false;
}


/*********** Chức năng chính của User **************/

/*******************************************************
*@Description: Hiển thị thông tin chi tiết tài khoản User.
*@Param:       strID – ID tài khoản cần xem.
*@Return:      none
*@Attention:   Đọc từ file [ID].txt và in ra console (bỏ header).
********************************************************/
inline void User::xemThongTinTaiKhoan(const string& strID)
{
    system("cls");

    Admin::setColor(9);
    cout << "\n\t\t============================================\n";
    cout << "\t\t||";
    Admin::setColor(12);
    cout << "           THONG TIN TAI KHOAN          ";
    Admin::setColor(9);
    cout << "||\n";
    cout << "\t\t============================================\n\n";
    Admin::setColor(7);

    bool bEsc = false;
    string infoFile = strID + ".txt";
    ifstream file(infoFile.c_str());
    if (!file.is_open())
    {
        Admin::setColor(12);
        cout << "\t\tKhong the mo file " << infoFile << "\n";
        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    string line;
    int skip = 3;
    while (skip-- > 0 && getline(file, line)) { }

    const string indent = "\t\t   ";

    while (getline(file, line))
    {
        if (line.empty()) continue;

        int pos = (int)line.find(':');
        if (pos != -1)
        {
            string label = line.substr(0, pos + 1);
            string value = line.substr(pos + 1);

            while (!value.empty() && value[0] == ' ')
                value.erase(0, 1);

            Admin::setColor(14);
            cout << indent << label << " ";

            Admin::setColor(7);
            cout << value << "\n";
        }
        else
        {
            Admin::setColor(7);
            cout << indent << line << "\n";
        }
    }

    file.close();

    cout << "\n";
    Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
}


/*******************************************************
*@Description: Rút tiền từ tài khoản User.
*@Param:       strID – ID tài khoản rút tiền.
*@Return:      none
*@Attention:   Số tiền rút là bội số 50000 và vẫn phải còn ≥ 50000.
********************************************************/
inline void User::rutTien(const string& strID)
{
    system("cls");
    Admin::setColor(9);
    cout << "\n\t\t============================================\n";
    cout << "\t\t||";
    Admin::setColor(12);
    cout << "                  RUT TIEN                ";
    Admin::setColor(9);
    cout << "||\n";
    cout << "\t\t============================================\n\n";
    Admin::setColor(7);

    bool bEsc = false;
    string infoFile = strID + ".txt";
    ifstream fin(infoFile.c_str());
    if (!fin.is_open())
    {
        Admin::setColor(12);
        cout << "\t\tKhong the mo file " << infoFile << "\n";
        Admin::setColor(7);

        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    vector<string> vLines;
    string line;
    long long soDuHienTai = -1;

    while (getline(fin, line))
    {
        vLines.push_back(line);

        if (line.find("So du:") != string::npos)
        {
            string soStr = "";
            for (int i = 0; i < (int)line.length(); i++)
            {
                char c = line[i];
                if (c >= '0' && c <= '9')
                    soStr.push_back(c);
            }
            if (!soStr.empty())
                soDuHienTai = stoll(soStr);
        }
    }
    fin.close();

    if (soDuHienTai < 0)
    {
        Admin::setColor(12);
        cout << "\t\tKhong doc duoc so du hien tai!\n";
        Admin::setColor(7);

        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    if (soDuHienTai < 100000)
    {
        Admin::setColor(12);
        cout << "\t\tSo du phai >= 100000 VND moi du dieu kien rut (giu lai 50000).\n";
        Admin::setColor(7);

        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    long long llSoTienRut = 0;
    string strTien;
    while (true)
    {
        Admin::setColor(14);
        cout << "\t\tSo du hien tai: ";
        Admin::setColor(7);
        cout << soDuHienTai;
        Admin::setColor(14);
        cout << " VND\n";
        Admin::setColor(14);
        strTien = Admin::inputWithESC(
            "\t\tNhap so tien can rut (So tien can rut phai la boi so cua 50000): ",
            bEsc);

        if (bEsc)
        {
            Admin::setColor(12);
            cout << "\n\t\tBan da huy thao tac rut tien.\n";
            Admin::setColor(7);
            Sleep(1000);
            return;
        }

        if (!Admin::isNumber(strTien))
        {
            Admin::setColor(4);
            cout << "\n\t\tChi duoc nhap so!\n\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        llSoTienRut = stoll(strTien);

        if (llSoTienRut < 50000)
        {
            Admin::setColor(4);
            cout << "\n\t\tSo tien rut phai >= 50000 VND!\n\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        if (llSoTienRut % 50000 != 0)
        {
            Admin::setColor(4);
            cout << "\n\t\tSo tien rut phai la BOI SO cua 50000!\n\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        if (llSoTienRut > soDuHienTai - 50000)
        {
            Admin::setColor(4);
            cout << "\n\t\tSo du khong du (sau khi rut phai con lai it nhat 50000)!\n\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        break;
    }

    // Xác nhận giao dịch
    char cXN;
    Admin::setColor(14);
    cout << "\n\t\tXac nhan rut ";
    Admin::setColor(7);
    cout << llSoTienRut;
    Admin::setColor(14);
    cout << " VND ? (Y/N): ";
    Admin::setColor(7);
    cin >> cXN;

    if (cXN != 'y' && cXN != 'Y')
    {
        Admin::setColor(12);
        cout << "\n\t\tGiao dich rut tien da bi huy.\n";
        Admin::setColor(7);
        Sleep(1000);
        return;
    }

    // Cập nhật số dư mới trong [ID].txt
    long long soDuMoi = soDuHienTai - llSoTienRut;

    for (int i = 0; i < (int)vLines.size(); i++)
    {
        if (vLines[i].find("So du:") != string::npos)
        {
            vLines[i] = "So du: " + to_string(soDuMoi) + " VND";
            break;
        }
    }

    ofstream fout(infoFile.c_str());
    if (fout.is_open())
    {
        for (int i = 0; i < (int)vLines.size(); i++)
            fout << vLines[i] << "\n";
        fout.close();
    }

    // Ghi lịch sử rút tiền
    time_t now = time(nullptr);
    tm localTm{};
    tm* pTm = std::localtime(&now);
    if (pTm) localTm = *pTm;

    char szTime[50];
    strftime(szTime, sizeof(szTime), "%d/%m/%Y %H:%M:%S", &localTm);

    string hisFile = "LichSu" + strID + ".txt";
    ofstream hOut(hisFile.c_str(), ios::app);
    if (hOut.is_open())
    {
        hOut << "Rut tien: " << llSoTienRut
             << " VND | So du sau rut: " << soDuMoi
             << " VND | Thoi gian: " << szTime << "\n";
        hOut.close();
    }

    Admin::setColor(2);
    cout << "\n\t\tRut tien thanh cong!\n";
    cout << "\t\tSo du hien tai: ";
    Admin::setColor(7);
    cout << soDuMoi;
    Admin::setColor(2);
    cout << " VND\n";

    Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
}


/*******************************************************
*@Description: Chuyển tiền từ một tài khoản User sang User khác.
*@Param:       strID – ID tài khoản gửi.
*@Return:      none
*@Attention:   Số tiền là bội số 50000, giữ lại tối thiểu 50000,
*              kiểm tra tồn tại & trạng thái tài khoản nhận.
********************************************************/
inline void User::chuyenTien(const string& strID)
{
    system("cls");
    Admin::setColor(9);
    cout << "\n\t\t============================================\n";
    cout << "\t\t||";
    Admin::setColor(12);
    cout << "             CHUYEN TIEN USER           ";
    Admin::setColor(9);
    cout << "||\n";
    cout << "\t\t============================================\n\n";
    Admin::setColor(7);

    bool bEsc = false;
    string infoGuiFile = strID + ".txt";
    ifstream finGui(infoGuiFile.c_str());
    if (!finGui.is_open())
    {
        Admin::setColor(12);
        cout << "\t\tKhong the mo file " << infoGuiFile << "\n";
        Admin::setColor(7);

        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    vector<string> vLinesGui;
    string line;
    string tenGui = "";
    long long soDuGui = -1;

    while (getline(finGui, line))
    {
        vLinesGui.push_back(line);

        int posColon = -1;
        for (int i = 0; i < (int)line.length(); i++)
        {
            if (line[i] == ':')
            {
                posColon = i;
                break;
            }
        }

        if (line.find("Ten") != string::npos && posColon != -1)
        {
            string s = line.substr(posColon + 1);
            while (!s.empty() && (s[0] == ' ' || s[0] == '\t'))
                s.erase(0, 1);
            tenGui = s;
        }

        if (line.find("So du:") != string::npos)
        {
            string soStr = "";
            for (int i = 0; i < (int)line.length(); i++)
            {
                char c = line[i];
                if (c >= '0' && c <= '9')
                    soStr.push_back(c);
            }
            if (!soStr.empty())
                soDuGui = stoll(soStr);
        }
    }
    finGui.close();

    if (soDuGui < 0)
    {
        Admin::setColor(12);
        cout << "\t\tKhong doc duoc so du hien tai!\n";
        Admin::setColor(7);
        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    if (soDuGui < 100000)
    {
        Admin::setColor(12);
        cout << "\t\tSo du phai >= 100000 VND moi du dieu kien chuyen (giu lai toi thieu 50000).\n";
        Admin::setColor(7);
        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    string strIDNhan;
    while (true)
    {
        Admin::setColor(14);
        strIDNhan = Admin::inputWithESC(
            "\t\tNhap ID tai khoan nhan (14 chu so): ",
            bEsc);
        Admin::setColor(7);

        if (bEsc)
        {
            Admin::setColor(4);
            cout << "\n\t\tBan da huy thao tac chuyen tien.\n";
            cout << "\n\t\tDang quay tro ve MENU USER...";
            Admin::setColor(7);
            Sleep(1000);
            return;
        }

        string onlyDigits = "";
        for (int i = 0; i < (int)strIDNhan.size(); i++)
        {
            unsigned char c = (unsigned char)strIDNhan[i];
            if (isdigit(c)) onlyDigits.push_back((char)c);
        }
        strIDNhan = onlyDigits;

        if (strIDNhan.length() != 14)
        {
            Admin::setColor(12);
            cout << "\n\t\tID phai co dung 14 chu so!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        if (strIDNhan == strID)
        {
            Admin::setColor(12);
            cout << "\n\t\tKhong the chuyen tien cho chinh ban!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        UserList ds;
        User::docDanhSachTheTu(FILE_THETU, ds);
        bool bExist    = false;
        bool bLockNhan = false;

        for (UserNode* p = ds.pHead; p != nullptr; p = p->pNext)
        {
            if (p->data.getID() == strIDNhan)
            {
                bExist    = true;
                bLockNhan = p->data.getLocked();
                break;
            }
        }

        if (!bExist)
        {
            Admin::setColor(4);
            cout << "\n\t\tTai khoan nhan khong ton tai!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        if (bLockNhan)
        {
            Admin::setColor(4);
            cout << "\n\t\tTai khoan nhan dang bi KHOA!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        break;
    }

    string infoNhanFile = strIDNhan + ".txt";
    ifstream finNhan(infoNhanFile.c_str());
    if (!finNhan.is_open())
    {
        Admin::setColor(12);
        cout << "\t\tKhong the mo file " << infoNhanFile << "\n";
        Admin::setColor(7);
        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    vector<string> vLinesNhan;
    string tenNhan = "";
    long long soDuNhan = -1;

    while (getline(finNhan, line))
    {
        vLinesNhan.push_back(line);

        int posColon = -1;
        for (int i = 0; i < (int)line.length(); i++)
        {
            if (line[i] == ':')
            {
                posColon = i;
                break;
            }
        }

        if (line.find("Ten") != string::npos && posColon != -1)
        {
            string s = line.substr(posColon + 1);
            while (!s.empty() && (s[0] == ' ' || s[0] == '\t'))
                s.erase(0, 1);
            tenNhan = s;
        }

        if (line.find("So du:") != string::npos)
        {
            string soStr = "";
            for (int i = 0; i < (int)line.length(); i++)
            {
                char c = line[i];
                if (c >= '0' && c <= '9')
                    soStr.push_back(c);
            }
            if (!soStr.empty())
                soDuNhan = stoll(soStr);
        }
    }
    finNhan.close();

    if (soDuNhan < 0)
    {
        Admin::setColor(12);
        cout << "\t\tKhong doc duoc so du tai khoan nhan!\n";
        Admin::setColor(7);

        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    long long llSoTien = 0;
    string strTien;

    while (true)
    {
        Admin::setColor(14);
        cout << "\n\t\tSo du hien tai: ";
        Admin::setColor(7);
        cout << soDuGui;
        Admin::setColor(14);
        cout << " VND\n";
        cout << "\t\tNhap so tien can chuyen (So tien can chuyen phai la boi so cua 50000): ";
        Admin::setColor(7);
        cin >> strTien;

        if (!Admin::isNumber(strTien))
        {
            Admin::setColor(12);
            cout << "\n\t\tChi duoc nhap so!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        llSoTien = stoll(strTien);

        if (llSoTien < 50000)
        {
            Admin::setColor(12);
            cout << "\n\t\tSo tien chuyen phai >= 50000 VND!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        if (llSoTien % 50000 != 0)
        {
            Admin::setColor(4);
            cout << "\n\t\tSo tien chuyen phai la BOI SO cua 50000!\n";
            Admin::setColor(7);
            Sleep(1200);
            continue;
        }

        if (llSoTien > soDuGui - 50000)
        {
            Admin::setColor(4);
            cout << "\n\t\tSo du khong du (phai giu lai it nhat 50000 sau khi chuyen)!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        break;
    }

    char cXN;
    Admin::setColor(14);
    cout << "\n\t\tXac nhan chuyen ";
    cout << llSoTien;
    Admin::setColor(14);
    cout << " VND cho ID ";
    Admin::setColor(7);
    cout << strIDNhan;
    Admin::setColor(14);
    cout << " (";
    Admin::setColor(7);
    cout << tenNhan;
    Admin::setColor(14);
    cout << ") ? (Y/N): ";
    Admin::setColor(7);
    cin >> cXN;

    if (cXN != 'y' && cXN != 'Y')
    {
        Admin::setColor(4);
        cout << "\n\t\tGiao dich da bi huy.\n";
        Admin::setColor(7);
        Sleep(1000);
        return;
    }

    long long soDuGuiMoi  = soDuGui  - llSoTien;
    long long soDuNhanMoi = soDuNhan + llSoTien;

    for (int i = 0; i < (int)vLinesGui.size(); i++)
    {
        if (vLinesGui[i].find("So du:") != string::npos)
        {
            vLinesGui[i] = "So du: " + to_string(soDuGuiMoi) + " VND";
            break;
        }
    }
    for (int i = 0; i < (int)vLinesNhan.size(); i++)
    {
        if (vLinesNhan[i].find("So du:") != string::npos)
        {
            vLinesNhan[i] = "So du: " + to_string(soDuNhanMoi) + " VND";
            break;
        }
    }

    ofstream foutGui(infoGuiFile.c_str());
    if (foutGui.is_open())
    {
        for (int i = 0; i < (int)vLinesGui.size(); i++)
            foutGui << vLinesGui[i] << "\n";
        foutGui.close();
    }

    ofstream foutNhan(infoNhanFile.c_str());
    if (foutNhan.is_open())
    {
        for (int i = 0; i < (int)vLinesNhan.size(); i++)
            foutNhan << vLinesNhan[i] << "\n";
        foutNhan.close();
    }

    time_t now = time(nullptr);
    tm localTm{};
    tm* pTm = std::localtime(&now);
    if (pTm) localTm = *pTm;

    char szTime[50];
    strftime(szTime, sizeof(szTime), "%d/%m/%Y %H:%M:%S", &localTm);

    string hisGui  = "LichSu" + strID + ".txt";
    string hisNhan = "LichSu" + strIDNhan + ".txt";

    ofstream hGui(hisGui.c_str(), ios::app);
    if (hGui.is_open())
    {
        hGui << "Chuyen den: " << strIDNhan << " - " << tenNhan
             << " | So tien: " << llSoTien << " VND | Thoi gian: " << szTime << "\n";
        hGui.close();
    }

    ofstream hNhan(hisNhan.c_str(), ios::app);
    if (hNhan.is_open())
    {
        hNhan << "Nhan tu: " << strID << " - " << tenGui
              << " | So tien: " << llSoTien << " VND | Thoi gian: " << szTime << "\n";
        hNhan.close();
    }

    Admin::setColor(2);
    cout << "\n\t\tChuyen tien thanh cong!\n";
    Admin::setColor(14);
    cout << "\t\tSo du hien tai: ";
    Admin::setColor(7);
    cout << soDuGuiMoi;
    Admin::setColor(14);
    cout << " VND\n";

    Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
}


/*******************************************************
*@Description: Xem lịch sử giao dịch của tài khoản User.
*@Param:       strID – ID tài khoản.
*@Return:      none
*@Attention:   Đọc file LichSu[ID].txt và in từng dòng.
********************************************************/
inline void User::xemNoiDungGiaoDich(const string& strID)
{
    system("cls");
    Admin::setColor(9);
    cout << "\n\t\t============================================\n";
    cout << "\t\t||";
    Admin::setColor(12);
    cout << "           LICH SU GIAO DICH            ";
    Admin::setColor(9);
    cout << "||\n";
    cout << "\t\t============================================\n\n";
    Admin::setColor(7);

    bool bEsc = false;
    string hisFile = "LichSu" + strID + ".txt";
    ifstream fin(hisFile.c_str());
    if (!fin.is_open())
    {
        Admin::setColor(12);
        cout << "\t\tKhong tim thay lich su giao dich ("
             << hisFile << ").\n";
        cout << "\t\tCo the ban chua thuc hien giao dich nao.\n";
        Admin::setColor(7);
        cout << "\n";

        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    string line;
    bool bEmpty = true;

    while (getline(fin, line))
    {
        if (line.size() == 0) continue;
        bEmpty = false;
        cout << "\t" << line << "\n";
    }
    fin.close();

    if (bEmpty)
    {
        Admin::setColor(14);
        cout << "\t\tHien tai ban chua co giao dich nao.\n";
        Admin::setColor(7);
    }

    cout << "\n";
    Admin::setColor(14);
    Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
}


/*******************************************************
*@Description: Đổi mã PIN cho tài khoản User.
*@Param:       strID – ID tài khoản cần đổi PIN.
*@Return:      none
*@Attention:   Kiểm tra PIN hiện tại; PIN mới phải 6 chữ số,
*              khác PIN cũ và khác PIN mặc định 123456.
********************************************************/
inline void User::doiMaPin(const string& strID)
{
    system("cls");
    Admin::setColor(9);
    cout << "\n\t\t============================================\n";
    cout << "\t\t||";
    Admin::setColor(12);
    cout << "              DOI MA PIN USER           ";
    Admin::setColor(9);
    cout << "||\n";
    cout << "\t\t============================================\n\n";
    Admin::setColor(7);

    bool bEsc = false;

    UserList ds;
    User::docDanhSachTheTu(FILE_THETU, ds);

    bool   bFound        = false;
    bool   bLocked       = false;
    string strPinHienTai = "";

    for (UserNode* p = ds.pHead; p != nullptr; p = p->pNext)
    {
        if (p->data.getID() == strID)
        {
            bFound        = true;
            bLocked       = p->data.getLocked();
            strPinHienTai = p->data.getPin();
            break;
        }
    }

    if (!bFound)
    {
        Admin::setColor(12);
        cout << "\t\tLoi: Khong tim thay tai khoan trong he thong!\n";
        Admin::setColor(7);
        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    if (bLocked)
    {
        Admin::setColor(4);
        cout << "\t\tTai khoan dang bi KHOA, khong the doi PIN!\n";
        Admin::setColor(7);
        Admin::escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    string strPinCu, strPinMoi, strConfirm;
    int    iErrorOld = 0;

    while (true)
    {
        system("cls");
        Admin::setColor(9);
        cout << "\n\t\t============================================\n";
        cout << "\t\t||";
        Admin::setColor(12);
        cout << "              DOI MA PIN USER           ";
        Admin::setColor(9);
        cout << "||\n";
        cout << "\t\t============================================\n\n";
        Admin::setColor(7);

        Admin::setColor(14);
        cout << "\t\tNhap PIN hien tai: ";
        Admin::setColor(7);
        strPinCu = Admin::pinHidden();

        if (strPinCu == strPinHienTai)
        {
            break;
        }

        iErrorOld++;
        Admin::setColor(12);
        cout << "\n\t\tPIN hien tai khong chinh xac!\n";
        Admin::setColor(7);

        if (iErrorOld >= 3)
        {
            Admin::setColor(4);
            cout << "\t\tBan da nhap sai PIN hien tai 3 lan. Huy thao tac doi PIN.\n";
            Admin::setColor(7);
            Sleep(1500);
            return;
        }
        Admin::setColor(4);
        cout << "\t\tBan con " << (3 - iErrorOld) << " lan de thu.\n";
        Admin::setColor(7);
        Sleep(1000);
    }

    while (true)
    {
        system("cls");
        Admin::setColor(9);
        cout << "\n\t\t============================================\n";
        cout << "\t\t||";
        Admin::setColor(12);
        cout << "              DOI MA PIN USER           ";
        Admin::setColor(9);
        cout << "||\n";
        cout << "\t\t============================================\n\n";
        Admin::setColor(7);

        Admin::setColor(14);
        cout << "\t\tPIN hien tai da duoc xac nhan.\n";
        cout << "\t\tVui long nhap PIN moi (6 chu so, khac PIN cu & khac 123456).\n\n";

        cout << "\t\tNhap PIN moi: ";
        Admin::setColor(7);
        strPinMoi = Admin::pinHidden();

        Admin::setColor(14);
        cout << "\t\tNhap lai PIN moi: ";
        Admin::setColor(7);
        strConfirm = Admin::pinHidden();

        if (strPinMoi != strConfirm)
        {
            Admin::setColor(4);
            cout << "\n\t\tHai lan nhap PIN khong giong nhau!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        bool bAllDigit = true;
        if (strPinMoi.length() != 6)
        {
            bAllDigit = false;
        }
        else
        {
            for (int i = 0; i < (int)strPinMoi.length(); i++)
            {
                if (!isdigit((unsigned char)strPinMoi[i]))
                {
                    bAllDigit = false;
                    break;
                }
            }
        }

        if (!bAllDigit)
        {
            Admin::setColor(12);
            cout << "\n\t\tPIN moi phai GOM DUNG 6 CHU SO!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        if (strPinMoi == strPinHienTai)
        {
            Admin::setColor(12);
            cout << "\n\t\tPIN moi khong duoc giong PIN cu!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        if (strPinMoi == "123456")
        {
            Admin::setColor(12);
            cout << "\n\t\tPIN moi khong duoc la PIN mac dinh (123456)!\n";
            Admin::setColor(7);
            Sleep(1000);
            continue;
        }

        break;
    }

    ofstream ofOut(FILE_THETU.c_str());
    if (!ofOut.is_open())
    {
        Admin::setColor(12);
        cout << "\n\t\tLoi: Khong the ghi file " << FILE_THETU << "!\n";
        Admin::setColor(7);
        Sleep(1000);
        return;
    }

    for (UserNode* p = ds.pHead; p != nullptr; p = p->pNext)
    {
        if (p->data.getID() == strID)
            ofOut << p->data.getID() << " " << strPinMoi
                  << " " << (p->data.getLocked() ? 0 : 1) << "\n";
        else
            ofOut << p->data.getID() << " " << p->data.getPin()
                  << " " << (p->data.getLocked() ? 0 : 1) << "\n";
    }
    ofOut.close();

    // Ghi lại file [ID].txt
    string infoFile = strID + ".txt";
    ifstream fin(infoFile.c_str());

    if (fin.is_open())
    {
        vector<string> vLines;
        string line;

        while (getline(fin, line))
            vLines.push_back(line);

        fin.close();

        for (int i = 0; i < (int)vLines.size(); i++)
        {
            if (vLines[i].find("PIN") != string::npos)
            {
                vLines[i] = "PIN: " + strPinMoi;
                break;
            }
        }

        ofstream fout(infoFile.c_str());
        if (fout.is_open())
        {
            for (int i = 0; i < (int)vLines.size(); i++)
                fout << vLines[i] << "\n";
            fout.close();
        }
    }

    Admin::setColor(2);
    cout << "\n\t\tDoi PIN thanh cong!\n";
    Admin::setColor(7);
    Sleep(1000);
}
