#pragma once
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

const string FILE_THETU  = "TheTu.txt";
const string FILE_ADMIN  = "Admin.txt";

//================= KHAI BÁO TRƯỚC =================
class Admin;

// Template danh sách liên kết dùng chung (Admin, User,...)
template<typename T>
class LinkedList;

using AdminList = LinkedList<Admin>;

//================= LỚP ADMIN ======================
class Admin {
private:
    string strID;
    string strPin;
    bool   bLocked = false;

public:
    //================= Constructor / Destructor =================
    Admin() {}

    Admin(const string& _strID, const string& _strPin)
        : strID(_strID), strPin(_strPin), bLocked(false) {}

    ~Admin() {}

    //================= Getter - Setter =================
    string getID()     const { return strID; }
    string getPin()    const { return strPin; }
    bool   getLocked() const { return bLocked; }

    void setLocked(bool bVal)      { bLocked = bVal; }
    void setID(const string& id)   { strID = id; }
    void setPin(const string& pin) { strPin = pin; }

    //================= Operator >> =================
    friend istream& operator >> (istream& is, Admin &a) {
        is >> a.strID >> a.strPin;
        return is;
    }

    //================= HÀM BỔ TRỢ CHO ADMIN =================
    static string inputWithESC(const string& prompt, bool &bEscPressed);
    static string escPauseScreen(const string& prompt, bool &bEscPressed);

    static void   setColor(int iColor);

    static vector<Admin> docDanhSachAdmin(const string& filePath);
    static bool kiemTraDangNhap(const string& filePath,
                                const string& strIDIn,
                                const string& strPinIn);

    static string pinHidden();

    static void docDanhSachTheTu(const string& filePath, AdminList &listOut);

    static bool isNumber(const string& s);

    //================= CHỨC NĂNG CHÍNH =================
    static void xemDanhSachTaiKhoan();
    static void themTaiKhoan();
    static void xoaTaiKhoan();
    static void moTaiKhoan();

    static void adminMenu();
    static bool dangNhapUI(const string& filePath);
};

//================= TEMPLATE LINKED LIST DÙNG CHUNG =================
template<typename T>
class LinkedList {
public:
    struct Node {
        T     data;
        Node* next;
        Node(const T& d) : data(d), next(nullptr) {}
    };

    Node* head;
    Node* tail;
    int   size;

    LinkedList() : head(nullptr), tail(nullptr), size(0) {}

    ~LinkedList() {
        clear();
    }

    void clear() {
        while (head) {
            Node* tmp = head;
            head = head->next;
            delete tmp;
        }
        tail = nullptr;
        size = 0;
    }

    bool isEmpty() const { return head == nullptr; }
    int  getSize() const { return size; }

    // Thêm vào cuối
    void addTail(const T& val) {
        Node* pNew = new Node(val);
        if (!head) {
            head = tail = pNew;
        } else {
            tail->next = pNew;
            tail       = pNew;
        }
        ++size;
    }

    // Hai hàm dưới chỉ compile được nếu T có getID()
    T* findByID(const string& id) {
        Node* p = head;
        while (p) {
            if (p->data.getID() == id)
                return &p->data;
            p = p->next;
        }
        return nullptr;
    }

    bool removeByID(const string& id) {
        if (!head) return false;

        // xóa đầu
        if (head->data.getID() == id) {
            Node* tmp = head;
            head = head->next;
            if (!head) tail = nullptr;
            delete tmp;
            --size;
            return true;
        }

        // xóa giữa/cuối
        Node* p = head;
        while (p->next) {
            if (p->next->data.getID() == id) {
                Node* tmp = p->next;
                p->next = tmp->next;
                if (tmp == tail) tail = p;
                delete tmp;
                --size;
                return true;
            }
            p = p->next;
        }
        return false;
    }
};

//================= CÁC HÀM THÀNH VIÊN ADMIN =================

/*******************************************************
*@Description: Nhập chuỗi ký tự, có thể nhấn ESC để hủy thao tác.
*@Param:       prompt – nội dung hướng dẫn;
*              bEscPressed – trả về true nếu người dùng nhấn ESC.
*@Return:      Chuỗi người dùng nhập, hoặc "" nếu nhấn ESC.
*@Attention:   Hỗ trợ BACKSPACE và kết thúc bằng ENTER.
********************************************************/
string Admin::inputWithESC(const string& prompt, bool &bEscPressed)
{
    bEscPressed = false;
    cout << prompt;

    setColor(7);
    string strResult;
    char c;

    while (true)
    {
        c = _getch();

        if (c == 27)   // ESC
        {
            bEscPressed = true;
            setColor(4);
            cout << "\n\t\tDa huy thao tac.\n";
            Sleep(800);
            return "";
        }
        else if (c == 13)  // ENTER
        {
            cout << endl;
            break;
        }
        else if (c == 8)   // Backspace
        {
            if (!strResult.empty())
            {
                strResult.pop_back();
                cout << "\b \b";
            }
        }
        else
        {
            cout << c;
            strResult.push_back(c);
        }
    }

    return strResult;
}

/*******************************************************
*@Description: Tạm dừng màn hình, chờ người dùng nhấn phím.
*@Param:       prompt – thông báo hiển thị;
*             bEscPressed – true nếu người dùng nhấn ESC.
*@Return:      Chuỗi nhập (thường không dùng).
*@Attention:   Dùng để “pause” rồi quay về màn hình trước.
********************************************************/
string Admin::escPauseScreen(const string& prompt, bool &bEscPressed)
{
    bEscPressed = false;
    cout << prompt;

    setColor(7);
    string strResult;
    char c;

    while (true)
    {
        c = _getch();

        if (c == 27)   // ESC
        {
            bEscPressed = true;
            setColor(2);
            cout << "\n\t\tDang quay tro ve...\n";
            Sleep(800);
            return "";
        }
        else if (c == 13)  // ENTER
        {
            cout << endl;
            break;
        }
        else if (c == 8)   // Backspace
        {
            if (!strResult.empty())
            {
                strResult.pop_back();
                cout << "\b \b";
            }
        }
        else
        {
            cout << c;
            strResult.push_back(c);
        }
    }

    return strResult;
}

/*******************************************************
*@Description: Đổi màu chữ trên console.
*@Param:       iColor – mã màu Windows console.
*@Return:      none
********************************************************/
void Admin::setColor(int iColor)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), iColor);
}

/*******************************************************
*@Description: Đọc danh sách admin từ file cấu hình.
*@Param:       filePath – đường dẫn đến FILE_ADMIN.
*@Return:      vector<Admin> chứa toàn bộ admin đọc được.
*@Attention:   Nếu không mở được file sẽ trả về vector rỗng.
********************************************************/
vector<Admin> Admin::docDanhSachAdmin(const string& filePath)
{
    vector<Admin> vAdminList;
    ifstream ifAdmin(filePath);

    if (!ifAdmin.is_open()) {
        cerr << "Khong the mo file: " << filePath << endl;
        return vAdminList;
    }

    string strFileID, strFilePin;
    while (ifAdmin >> strFileID >> strFilePin)
    {
        Admin a(strFileID, strFilePin);
        vAdminList.push_back(a);
    }

    ifAdmin.close();
    return vAdminList;
}

/*******************************************************
*@Description: Kiểm tra thông tin đăng nhập admin.
*@Param:       filePath – đường dẫn FILE_ADMIN;
*              strIDIn – ID nhập vào;
*              strPinIn – PIN nhập vào.
*@Return:      true nếu ID và PIN trùng với dữ liệu trong file.
********************************************************/
bool Admin::kiemTraDangNhap(const string& filePath,
                            const string& strIDIn,
                            const string& strPinIn)
{
    vector<Admin> vList = docDanhSachAdmin(filePath);

    for (int i = 0; i < (int)vList.size(); i++) {
        const Admin &a = vList[i];

        if (a.getID() == strIDIn && a.getPin() == strPinIn)
            return true;
    }

    return false;
}

/*******************************************************
*@Description: Nhập PIN từ bàn phím và hiển thị dạng '*'.
*@Param:       none
*@Return:      Chuỗi PIN người dùng nhập.
*@Attention:   Chỉ nhận ký tự số; hỗ trợ BACKSPACE và ENTER.
********************************************************/
string Admin::pinHidden()
{
    string strPinInput;
    char ch;
    while (true)
    {
        ch = _getch();

        if (ch == 13) break;
        else if (ch == 8 && !strPinInput.empty())
        {
            cout << "\b \b";
            strPinInput.pop_back();
        }
        else if (isdigit((unsigned char)ch))
        {
            strPinInput.push_back(ch);
            cout << "*";
        }
    }
    cout << endl;
    return strPinInput;
}

/*******************************************************
*@Description: Đọc danh sách tài khoản User từ FILE_THETU.
*@Param:       filePath – đường dẫn FILE_THETU;
*              listOut – danh sách liên kết cần ghi dữ liệu.
*@Return:      none
*@Attention:   Gọi clear() trước khi nạp dữ liệu mới.
********************************************************/
void Admin::docDanhSachTheTu(const string& filePath, AdminList &listOut)
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
        Admin a(strFileID, strFilePin);
        // 0 = KHOA, 1 = DANG HOAT DONG
        a.setLocked(iStatus == 0);
        listOut.addTail(a);
    }

    ifUser.close();
}

/*******************************************************
*@Description: Kiểm tra chuỗi có phải toàn ký tự số.
*@Param:       s – chuỗi cần kiểm tra.
*@Return:      true nếu tất cả ký tự là số, ngược lại false.
********************************************************/
bool Admin::isNumber(const string& s)
{
    if (s.empty()) return false;
    for (char c : s)
        if (!isdigit((unsigned char)c))
            return false;
    return true;
}

/*************** CÁC CHỨC NĂNG CHÍNH CỦA ADMIN *************/

/*******************************************************
*@Description: Đọc và in danh sách toàn bộ tài khoản User.
*@Param:       none
*@Return:      none
*@Attention:   Dùng AdminList để duyệt FILE_THETU và in bảng.
********************************************************/
void Admin::xemDanhSachTaiKhoan()
{
    system("cls");
    AdminList list;
    docDanhSachTheTu(FILE_THETU, list);

    Admin::setColor(9);
    cout << "\n\t\t=====================================================\n";
    cout << "\t\t||";
    Admin::setColor(12);
    cout << "           DANH SACH TAI KHOAN NGAN HANG         ";
    Admin::setColor(9);
    cout << "||\n";
    cout << "\t\t=====================================================\n\n";

    cout << "\t\t" << left << setw(6)  << "STT"
                 << setw(20) << "ID"
                 << setw(12) << "PIN"
                 << setw(18) << "TRANG THAI" << "\n";
    cout << "\t\t----------------------------------------------------\n";

    bool bEsc = false;
    setColor(7);
    int stt = 1;
    for (AdminList::Node* p = list.head; p != nullptr; p = p->next)
    {
        const Admin &acc = p->data;
        cout << "\t\t" << left << setw(6)  << stt++
             << setw(20) << acc.getID()
             << setw(12) << acc.getPin();

        if(!acc.getLocked())
            setColor(2);
        else
            setColor(4);

        cout << setw(18) << (acc.getLocked() ? "KHOA" : "DANG HOAT DONG");
        setColor(7);
        cout<< "\n";
    }
    setColor(14);
    cout << "\n\t\tTong so tai khoan: " ;
    setColor(2);
    cout << list.getSize() << "\n";
    setColor(14);
    escPauseScreen("\n\t\tNhan phim bat ky de tiep tuc . . .",bEsc);
}

/*******************************************************
*@Description: Tạo mới tài khoản User trong hệ thống.
*@Param:       none
*@Return:      none
*@Attention:   Ghi thêm vào FILE_THETU + tạo file <ID>.txt và LichSu<ID>.txt.
********************************************************/
void Admin::themTaiKhoan()
{
    system("cls");
    bool bEsc = false;
    string strId;
    string strTen;
    string strLoaiTien = "VND";
    long long llSoDu = 0;
    string strPinMacDinh = "123456";


    setColor(9);
    cout << "\n\t\t============================================\n";
    cout << "\t\t||"; setColor(12);
    cout << "             THEM TAI KHOAN MOI         ";
    setColor(9); cout << "||\n";
    cout << "\t\t============================================\n\n";

    while (true)
    {
        setColor(14);
        strId = inputWithESC("\t\tNhap ID (14 chu so): ", bEsc);
        if (bEsc)
        {
            setColor(12);
            cout << "\n\t\tDang quay ve MENU ADMIN...\n";
            Sleep(800); return;
        }

        string digits;
        for (char c : strId)
            if (isdigit((unsigned char)c)) digits.push_back(c);
        strId = digits;

        if (strId.length() != 14)
        {
            setColor(4);
            cout << "\n\t\tID phai dung 14 chu so!\n";
            Sleep(1000); continue;
        }

        ifstream ifCheck(FILE_THETU);
        if (!ifCheck.is_open())
        {
            setColor(4);
            cout << "\n\t\tKhong the mo file " << FILE_THETU << "!\n";
            Sleep(1000); return;
        }

        string id, pin; int st; bool exists = false;
        while (ifCheck >> id >> pin >> st)
            if (id == strId) { exists = true; break; }
        ifCheck.close();

        if (exists)
        {
            setColor(4);
            cout << "\n\t\tID nay da ton tai!\n";
            Sleep(1000); continue;
        }

        break;
    }

    while (true)
    {
        setColor(14);
        strTen = inputWithESC("\t\tNhap ten tai khoan: ", bEsc);
        if (bEsc)
        {
            setColor(12);
            cout << "\n\t\tDang quay ve MENU ADMIN...\n";
            Sleep(800); return;
        }

        bool ok = !strTen.empty();
        for (char c : strTen)
            if (!(isalpha((unsigned char)c) || c == ' ')) ok = false;

        if (!ok)
        {
            setColor(4);
            cout << "\n\t\tTen chi duoc chua chu cai!\n";
            Sleep(1000); continue;
        }

        break;
    }

    while (true)
    {
        setColor(14);
        strLoaiTien = inputWithESC("\t\tNhap loai tien te (VND): ", bEsc);
        if (bEsc)
        {
            setColor(12);
            cout << "\n\t\tDang quay ve MENU ADMIN...\n";
            Sleep(800); return;
        }

        string cleaned;
        for (char c : strLoaiTien)
            if (!isspace((unsigned char)c))
                cleaned.push_back(toupper((unsigned char)c));

        if (cleaned != "VND")
        {
            setColor(4);
            cout << "\n\t\tChi ho tro VND!\n";
            Sleep(1000); continue;
        }

        strLoaiTien = "VND";
        break;
    }

    string strSoDu;
    while (true)
    {
        setColor(14);
        strSoDu = inputWithESC("\t\tNhap so du ban dau (>= 50000): ", bEsc);
        if (bEsc)
        {
            setColor(12);
            cout << "\n\t\tDang quay ve MENU ADMIN...\n";
            Sleep(800); return;
        }

        bool ok = !strSoDu.empty();
        for (char c : strSoDu)
            if (!isdigit((unsigned char)c)) ok = false;

        if (!ok)
        {
            setColor(4);
            cout << "\n\t\tChi duoc nhap so!\n";
            Sleep(1000); continue;
        }

        llSoDu = stoll(strSoDu);
        if (llSoDu < 50000)
        {
            setColor(4);
            cout << "\n\t\tSo du phai >= 50000 VND!\n";
            Sleep(1000); continue;
        }

        break;
    }

    ofstream ofAppend(FILE_THETU, ios::app);
    ofAppend << strId << " " << strPinMacDinh << " " << 1 << "\n";
    ofAppend.close();

    ofstream ofUser(strId + ".txt");
    if (ofUser.is_open())
    {
        ofUser << "============================================\n";
        ofUser << "||             THONG TIN TAI KHOAN        ||\n";
        ofUser << "============================================\n";
        ofUser << "ID: " << strId << "\n";
        ofUser << "Ten: " << strTen << "\n";
        ofUser << "So du: " << llSoDu << "\n";
        ofUser << "Loai tien te: " << strLoaiTien << "\n";
        ofUser << "PIN mac dinh: 123456\n";
        ofUser << "Trang thai: DANG HOAT DONG\n";
        ofUser.close();
    }

    time_t now = time(nullptr);
    tm* t = localtime(&now);
    char buf[50];
    strftime(buf, sizeof(buf), "%d/%m/%Y %H:%M:%S", t);

    ofstream ofHis("LichSu" + strId + ".txt");
    ofHis << "=== Lich su giao dich cua tai khoan " << strId << " ===\n";
    ofHis << "Tao luc: " << buf << "\n";
    ofHis.close();

    setColor(9);
    cout << "\n\t\t============================================\n";
    cout << "\t\t||"; setColor(10);
    cout << "        TAO TAI KHOAN THANH CONG!        ";
    setColor(9); cout << "||\n";
    cout << "\t\t============================================\n";

    setColor(9); cout << "\t\tID: "; setColor(7); cout << strId << "\n";
    setColor(9); cout << "\t\tTen: "; setColor(7); cout << strTen << "\n";
    setColor(9); cout << "\t\tSo du: "; setColor(7); cout << llSoDu << " " << strLoaiTien << "\n";
    setColor(9); cout << "\t\tPIN mac dinh: "; setColor(7); cout << "123456\n";
    setColor(9); cout << "\t\tTrang thai: "; setColor(2); cout << "DANG HOAT DONG\n";

    setColor(12);
    cout << "\n\t\tDang quay ve MENU ADMIN...\n";
    Sleep(3000);
}

/*******************************************************
*@Description: Xóa tài khoản User khỏi hệ thống.
*@Param:       none
*@Return:      none
*@Attention:   Xóa trong FILE_THETU + file thông tin + file lịch sử.
********************************************************/
void Admin::xoaTaiKhoan()
{
    bool bEsc    = false;
    string strIDDel;

    while (true)
    {
        system("cls");
        setColor(9);
        cout << "\n\t\t============================================\n";
        cout << "\t\t||";
        setColor(12);
        cout << "             XOA TAI KHOAN              ";
        setColor(9);
        cout << "||\n";
        cout << "\t\t============================================\n";


        setColor(14);
        strIDDel = inputWithESC("\t\tNhap ID can xoa (14 chu so): ", bEsc);
        setColor(7);
        if (bEsc)
        {
            setColor(12);
            cout << "\n\t\tDang quay ve MENU ADMIN...\n";
            Sleep(800);
            return;
        }

        string strOnlyDigits;
        for (int i = 0; i < (int)strIDDel.size(); i++)
        {
            unsigned char c = (unsigned char)strIDDel[i];
            if (isdigit(c)) strOnlyDigits.push_back((char)c);
        }
        strIDDel = strOnlyDigits;

        if (strIDDel.length() != 14)
        {
            setColor(4);
            cout << "\n\t\tLoi: ID phai co dung 14 chu so!\n";
            Sleep(1000);
            continue;
        }

        AdminList list;
        docDanhSachTheTu(FILE_THETU, list);
        if (list.isEmpty())
        {
            setColor(4);
            cout << "\n\t\tKhong co tai khoan nao trong he thong.\n";

            setColor(14);
            cout << "\n\t\tNhan phim bat ky de tiep tuc . . .";
            cin.ignore();
            cin.get();
            return;
        }

        bool bDeleted = list.removeByID(strIDDel);

        if (!bDeleted)
        {
            setColor(4);
            cout << "\n\t\tKhong tim thay ID trong he thong, vui long nhap lai!\n";
            Sleep(1000);
            continue;
        }

        ofstream ofOut(FILE_THETU);
        if (!ofOut.is_open())
        {
            setColor(4);
            cout << "\n\t\tLoi: Khong the ghi file " << FILE_THETU << "!\n";
            Sleep(1000);
            return;
        }

        for (AdminList::Node* p = list.head; p != nullptr; p = p->next)
        {
            const Admin &acc = p->data;
            ofOut << acc.getID() << " " << acc.getPin() << " "
                  << (acc.getLocked() ? 0 : 1) << "\n";
        }
        ofOut.close();

        string strInfoFile = strIDDel + ".txt";
        if (remove(strInfoFile.c_str()) != 0)
        {
            Admin::setColor(6);
            cout << "\n\t\tCanh bao: Khong the xoa file thong tin " << strInfoFile
                 << " (co the file khong ton tai).\n";
        }

        string strHistoryFile = "LichSu" + strIDDel + ".txt";
        remove(strHistoryFile.c_str());

        setColor(2);
        cout << "\n\t\tDa xoa tai khoan co ID: " << strIDDel << " thanh cong.\n";
        setColor(7);

        setColor(14);
        cout << "\n\t\tNhan phim bat ky de tiep tuc . . .";
        cin.ignore();
        cin.get();
        break;
    }
}

/*******************************************************
*@Description: Mở khóa tài khoản User đang bị KHOA.
*@Param:       none
*@Return:      none
*@Attention:   Reset PIN về 123456 và cập nhật FILE_THETU + <ID>.txt.
********************************************************/
void Admin::moTaiKhoan()
{
    system("cls");
    setColor(9);
    cout << "\n\t\t============================================\n";
    cout << "\t\t||";
    setColor(12);
    cout << "             MO KHOA TAI KHOAN          ";
    setColor(9);
    cout << "||\n";
    cout << "\t\t============================================\n";

    AdminList list;
    docDanhSachTheTu(FILE_THETU, list);
    if (list.isEmpty())
    {
        setColor(4);
        cout << "\n\t\tKhong co tai khoan nao trong he thong.\n";
        setColor(7);
        setColor(14);
        cout << "\n\t\tNhan phim bat ky de tiep tuc . . .";
        cin.ignore();
        cin.get();
        return;
    }

    int sttKhoa = 0;
    setColor(11);
    cout << "\n\t\tDanh sach tai khoan dang bi KHOA:\n\n";
    setColor(7);

    cout << "\t\t" << left << setw(6) << "STT" << setw(20) << "ID" << "\n";
    cout << "\t\t-----------------------------------\n";

    for (AdminList::Node* p = list.head; p != nullptr; p = p->next)
    {
        if (p->data.getLocked())
        {
            sttKhoa++;
            setColor(4);
            cout << "\t\t" << left << setw(6) << sttKhoa
                 << setw(20) << p->data.getID() << "\n";
            setColor(7);
        }
    }

    bool bEsc = false;
    if (sttKhoa == 0)
    {
        setColor(2);
        cout << "\n\t\tHien khong co tai khoan nao dang bi KHOA.\n";
        setColor(7);
        escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
        return;
    }

    string strIDUnLock;

    while (true)
    {
        setColor(14);
        strIDUnLock = inputWithESC("\t\tNhap ID can mo khoa: ", bEsc);

        if (bEsc)
        {
            setColor(12);
            cout << "\n\t\tDang quay ve MENU ADMIN...\n";
            Sleep(800);
            return;
        }

        string digits;
        for (char c : strIDUnLock)
            if (isdigit((unsigned char)c)) digits.push_back(c);
        strIDUnLock = digits;

        if (strIDUnLock.length() != 14)
        {
            setColor(4);
            cout << "\n\t\tID phai dung 14 chu so!\n";
            Sleep(1000);
            continue;
        }

        Admin* pAcc = list.findByID(strIDUnLock);
        if (!pAcc)
        {
            setColor(4);
            cout << "\n\t\tKhong tim thay ID trong he thong!\n";
            Sleep(1000);
            continue;
        }

        if (!pAcc->getLocked())
        {
            setColor(2);
            cout << "\n\t\tTai khoan nay dang HOAT DONG!\n";
            Sleep(1000);
            continue;
        }

        // Mở khóa
        pAcc->setLocked(false);
        pAcc->setPin("123456");

        setColor(2);
        cout << "\n\t\tMo khoa thanh cong.\n";
        cout << "\n\t\tPin da duoc dat ve mac dinh (123456).\n";

        ofstream ofOut(FILE_THETU);
        for (AdminList::Node* p = list.head; p != nullptr; p = p->next)
        {
            Admin &acc = p->data;
            ofOut << acc.getID() << " "
                  << acc.getPin() << " "
                  << (acc.getLocked() ? 0 : 1) << "\n";
        }
        ofOut.close();

        string infoFile = strIDUnLock + ".txt";
        ifstream fin(infoFile.c_str());

        if (fin.is_open())
        {
            vector<string> lines;
            string line;

            while (getline(fin, line))
                lines.push_back(line);
            fin.close();

            for (int i = 0; i < (int)lines.size(); i++)
            {
                if (lines[i].find("Trang thai:") != string::npos)
                    lines[i] = "Trang thai: DANG HOAT DONG";

                if (lines[i].find("PIN") != string::npos)
                    lines[i] = "PIN: 123456";
            }

            ofstream fout(infoFile.c_str());
            for (int i = 0; i < (int) lines.size(); i++)
                fout << lines[i] << "\n";
            fout.close();
        }

        break;
    }

    escPauseScreen("\n\t\tNhan phim ESC de thoat. . .", bEsc);
}

/*******************************************************
*@Description: Hiển thị menu chính và xử lý lựa chọn của admin.
*@Param:       none
*@Return:      none
*@Attention:   Gọi các chức năng xem/thêm/xóa/mở khóa tài khoản.
********************************************************/
void Admin::adminMenu()
{
    int iChoice = -1;
    string strInput;

    do
    {
        system("cls");
        Admin::setColor(9);
        cout << "\n\t\t============================================\n";
        cout << "\t\t||";
        Admin::setColor(12);
        cout << "               MENU ADMIN               ";
        Admin::setColor(9);
        cout << "||\n";
        cout << "\t\t============================================\n";

        Admin::setColor(9);
        cout << "\t\t||  ";
        Admin::setColor(7);
        cout << "1. Xem danh sach tai khoan            ";
        Admin::setColor(9);
        cout << "||\n";

        cout << "\t\t||  ";
        Admin::setColor(7);
        cout << "2. Them tai khoan                     ";
        Admin::setColor(9);
        cout << "||\n";

        cout << "\t\t||  ";
        Admin::setColor(7);
        cout << "3. Xoa tai khoan                      ";
        Admin::setColor(9);
        cout << "||\n";

        cout << "\t\t||  ";
        Admin::setColor(7);
        cout << "4. Mo khoa tai khoan                  ";
        Admin::setColor(9);
        cout << "||\n";

        cout << "\t\t||  ";
        Admin::setColor(7);
        cout << "5. Thoat                              ";
        Admin::setColor(9);
        cout << "||\n";

        Admin::setColor(9);
        cout << "\t\t============================================\n";
        setColor(14);
        cout <<"\n\n\t\tHay nhap lua chon: ";
        setColor(7);

        Admin::setColor(7);
        cin >> strInput;

        if(!isNumber(strInput))
        {
            Admin::setColor(4);
            cout << "\n\t\tChi duoc nhap so (1-5)!";
            Sleep(1200);
            continue;
        }

        iChoice = stoi(strInput);

        switch(iChoice)
        {
        case 1:
            xemDanhSachTaiKhoan();
            break;
        case 2:
            themTaiKhoan();
            break;
        case 3:
            xoaTaiKhoan();
            break;
        case 4:
            moTaiKhoan();
            break;
        case 5:
            break;
        default:
            setColor(12);
            cout << "\n\tLua chon khong hop le!\n";
            setColor(7);
            Sleep(1000);
            break;
        }
    } while (iChoice != 5);
}

/*******************************************************
*@Description: Giao diện đăng nhập admin (nhập ID + PIN ẩn).
*@Param:       filePath – đường dẫn đến FILE_ADMIN.
*@Return:      true nếu đăng nhập thành công, false nếu sai quá 3 lần.
*@Attention:   Đăng nhập thành công sẽ gọi adminMenu().
********************************************************/
bool Admin::dangNhapUI(const string& filePath)
{
    int iErrorCount = 0;
    string strIDIn, strPinIn;

    system("cls");
    setColor(9);
    cout << "\n\t\t============================================\n";
    cout << "\t\t||";
    setColor(12);
    cout << "               DANG NHAP ADMIN          ";
    setColor(9);
    cout << "||\n";
    cout << "\t\t============================================\n\n";
    setColor(7);

    while (iErrorCount < 3)
    {

        setColor(14);
        cout << "\t\tUser: ";
        setColor(7);
        cin >> strIDIn;

        setColor(14);
        cout << "\t\tPin: ";
        setColor(7);
        strPinIn = pinHidden();

        bool bHopLe = kiemTraDangNhap(filePath, strIDIn, strPinIn);

        if (bHopLe)
        {
            system("cls");
            setColor(10);
            cout << "\n\t\tDang nhap thanh cong!\n";
            setColor(7);
            Sleep(800);
            adminMenu();
            return true;
        }
        else
        {
            ++iErrorCount;
            setColor(12);
            cout << "\n\t\tPIN/ID khong chinh xac!\n";
            cout << "\t\tBan con " << (3 - iErrorCount) << " lan de thu.\n\n";
            setColor(7);
            Sleep(1000);

        }
    }

    system("cls");
    setColor(12);
    cout << "\n\tBan da nhap Pin/ID sai qua 3 lan.";
    cout << "\n\tTro ve Menu chinh!.\n";
    setColor(7);
    Sleep(1000);
    return false;
}
