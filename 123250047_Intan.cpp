#include <iostream>
#include <string>
using namespace std;

string toLowerStr(string nama) {
    for (size_t i = 0; i < nama.length(); i++) {
        nama[i] = tolower(nama[i]);
    }
    return nama;
}

struct QueueNode {
    string pelanggan;
    QueueNode* next;
};

struct Menu {
    string nama;
    string namaLower;
    string kategori;
    int harga;
    bool tersedia;

    QueueNode* front;
    QueueNode* rear;

    Menu* left;
    Menu* right;
};

struct History {
    string aksi;
    string namaMenu;
    string pelanggan;
    bool statusSebelum;
    History* next;
};

Menu* root = nullptr;
History* top = nullptr;

void push(string aksi, string namaMenu, string pelanggan = "", bool status = true) {
    History* baru = new History;
    baru->aksi = aksi;
    baru->namaMenu = namaMenu;
    baru->pelanggan = pelanggan;
    baru->statusSebelum = status;
    baru->next = top;
    top = baru;
}

bool pop(History& data) {
    if (top == nullptr) {
        return false;
    }

    History* temp = top;
    data = *temp;
    top = top->next;
    delete temp;

    return true;
}

void enqueue(Menu* menu, string pelanggan) {
    QueueNode* baru = new QueueNode;
    baru->pelanggan = pelanggan;
    baru->next = nullptr;

    if (menu->front == nullptr) {
        menu->front = baru;
        menu->rear = baru;
    } else {
        menu->rear->next = baru;
        menu->rear = baru;
    }
}

bool dequeue(Menu* menu, string& pelanggan) {
    if (menu->front == nullptr) {
        return false;
    }

    QueueNode* temp = menu->front;
    pelanggan = temp->pelanggan;
    menu->front = temp->next;

    if (menu->front == nullptr) {
        menu->rear = nullptr;
    }

    delete temp;
    return true;
}

Menu* tambahMenu(Menu* node, string nama, string kategori, int harga) {
    string namaLower = toLowerStr(nama);

    if (node == nullptr) {
        Menu* baru = new Menu;
        baru->nama = nama;
        baru->namaLower = namaLower;
        baru->kategori = kategori;
        baru->harga = harga;
        baru->tersedia = true;
        baru->front = nullptr;
        baru->rear = nullptr;
        baru->left = nullptr;
        baru->right = nullptr;
        return baru;
    }

    if (namaLower < node->namaLower) {
        node->left = tambahMenu(node->left, nama, kategori, harga);
    } else if (namaLower > node->namaLower) {
        node->right = tambahMenu(node->right, nama, kategori, harga);
    }

    return node;
}

Menu* cariMenu(Menu* node, string nama) {
    string cari = toLowerStr(nama);

    while (node != nullptr) {
        if (cari == node->namaLower) {
            return node;
        } else if (cari < node->namaLower) {
            node = node->left;
        } else {
            node = node->right;
        }
    }

    return nullptr;
}

void tampilMenu(Menu* node) {
    if (node == nullptr) {
        return;
    }

    tampilMenu(node->left);

    cout << "- " << node->nama
         << " | Kategori: " << node->kategori
         << " | Harga: " << node->harga
         << " | Status: " << (node->tersedia ? "Tersedia" : "Habis")
         << endl;

    tampilMenu(node->right);
}

Menu* cariMin(Menu* node) {
    while (node != nullptr && node->left != nullptr) {
        node = node->left;
    }

    return node;
}

Menu* hapusMenuNode(Menu* node, string nama, bool& berhasil) {
    if (node == nullptr) {
        return nullptr;
    }

    string cari = toLowerStr(nama);

    if (cari < node->namaLower) {
        node->left = hapusMenuNode(node->left, nama, berhasil);
    } else if (cari > node->namaLower) {
        node->right = hapusMenuNode(node->right, nama, berhasil);
    } else {
        berhasil = true;

        if (node->left == nullptr) {
            Menu* temp = node->right;
            delete node;
            return temp;
        }

        if (node->right == nullptr) {
            Menu* temp = node->left;
            delete node;
            return temp;
        }

        Menu* temp = cariMin(node->right);

        node->nama = temp->nama;
        node->namaLower = temp->namaLower;
        node->kategori = temp->kategori;
        node->harga = temp->harga;
        node->tersedia = temp->tersedia;
        node->front = temp->front;
        node->rear = temp->rear;

        bool hapusBerhasil = false;
        node->right = hapusMenuNode(node->right, temp->nama, hapusBerhasil);
    }

    return node;
}

void tambahDataMenu() {
    string nama, kategori;
    int harga;

    cout << "Masukkan Nama Menu: ";
    getline(cin, nama);

    cout << "Masukkan Kategori Makanan: ";
    getline(cin, kategori);

    cout << "Masukkan Harga: ";
    cin >> harga;
    cin.ignore();

    if (cariMenu(root, nama) != nullptr) {
        cout << "Menu sudah terdaftar.\n";
        return;
    }

    root = tambahMenu(root, nama, kategori, harga);
    cout << "Menu berhasil ditambahkan\n";
}

void lihatMenu() {
    if (root == nullptr) {
        cout << "Belum ada menu yang terdaftar.\n";
        return;
    }

    cout << "=== DAFTAR MENU RESTORAN ===\n";
    tampilMenu(root);
}

void catatPesanan() {
    string pelanggan, namaMenu;

    cout << "Masukkan Nama Pelanggan: ";
    getline(cin, pelanggan);

    cout << "Masukkan Nama Menu yang Dipesan: ";
    getline(cin, namaMenu);

    Menu* menu = cariMenu(root, namaMenu);

    if (menu == nullptr) {
        cout << "Menu tidak ditemukan.\n";
        return;
    }

    if (menu->tersedia) {
        cout << "Pesanan " << pelanggan << " berhasil dibuat\n";
        push("PESAN", menu->nama, pelanggan);
    } else {
        cout << "Menu sedang habis, " << pelanggan
             << " masuk ke dalam waiting list\n";

        enqueue(menu, pelanggan);
        push("WAITING", menu->nama, pelanggan);
    }
}

void ubahStatusMenu() {
    string namaMenu;

    cout << "Masukkan Nama Menu yang Ingin Diubah Statusnya: ";
    getline(cin, namaMenu);

    Menu* menu = cariMenu(root, namaMenu);

    if (menu == nullptr) {
        cout << "Menu tidak ditemukan.\n";
        return;
    }

    bool statusLama = menu->tersedia;
    push("STATUS", menu->nama, "", statusLama);

    if (menu->tersedia == false) {
        menu->tersedia = true;
        cout << "Menu tersedia kembali\n";

        string pelanggan;
        if (dequeue(menu, pelanggan)) {
            cout << "Pesanan langsung diberikan kepada "
                 << pelanggan << " dari waiting list\n";
        }
    } else {
        menu->tersedia = false;
        cout << "Menu " << menu->nama << " sekarang berstatus: Habis\n";
    }
}

void hapusMenu() {
    string namaMenu;
    bool berhasil = false;

    cout << "Masukkan Nama Menu yang Ingin Dihapus: ";
    getline(cin, namaMenu);

    root = hapusMenuNode(root, namaMenu, berhasil);

    if (berhasil) {
        cout << "Menu berhasil dihapus\n";
    } else {
        cout << "Menu tidak ditemukan untuk dihapus.\n";
    }
}

void undoAksi() {
    History data;

    if (!pop(data)) {
        cout << "Tidak ada aksi untuk di-undo\n";
        return;
    }

    if (data.aksi == "PESAN") {
        cout << "Undo: Pesanan menu " << data.namaMenu
             << " dibatalkan\n";
    } else if (data.aksi == "WAITING") {
        Menu* menu = cariMenu(root, data.namaMenu);

        if (menu != nullptr) {
            QueueNode* curr = menu->front;
            QueueNode* sebelum = nullptr;

            while (curr != nullptr) {
                if (curr->pelanggan == data.pelanggan) {
                    if (sebelum == nullptr) {
                        menu->front = curr->next;
                    } else {
                        sebelum->next = curr->next;
                    }

                    if (curr == menu->rear) {
                        menu->rear = sebelum;
                    }

                    delete curr;
                    break;
                }

                sebelum = curr;
                curr = curr->next;
            }
        }

        cout << "Undo: Pesanan menu " << data.namaMenu
             << " dibatalkan\n";
    } else if (data.aksi == "STATUS") {
        Menu* menu = cariMenu(root, data.namaMenu);

        if (menu != nullptr) {
            menu->tersedia = data.statusSebelum;
        }

        if (data.statusSebelum == false) {
            cout << "Undo: Menu " << data.namaMenu
                 << " kembali habis\n";
        } else {
            cout << "Undo: Menu " << data.namaMenu
                 << " kembali tersedia\n";
        }
    }
}

void tampilkanAntrian() {
    string namaMenu;

    cout << "Masukkan Nama Menu untuk Cek Antrian: ";
    getline(cin, namaMenu);

    Menu* menu = cariMenu(root, namaMenu);

    if (menu == nullptr) {
        cout << "Menu tidak ditemukan.\n";
        return;
    }

    if (menu->front == nullptr) {
        cout << "Antrian kosong\n";
        return;
    }

    QueueNode* temp = menu->front;

    while (temp != nullptr) {
        cout << temp->pelanggan << " menunggu menu "
             << menu->nama << endl;
        temp = temp->next;
    }
}

int main() {
    int pilihan;

    do {
        cout << "\n=========================================\n";
        cout << "   SISTEM MANAJEMEN PESANAN RESTORAN     \n";
        cout << "=========================================\n";
        cout << "1. Tambah Menu Makanan\n";
        cout << "2. Lihat Daftar Menu\n";
        cout << "3. Catat Pesanan Pelanggan\n";
        cout << "4. Ubah Status Menu (Tersedia / Habis)\n";
        cout << "5. Hapus Menu\n";
        cout << "6. Undo Aksi Terakhir\n";
        cout << "7. Lihat Antrian Pelanggan\n";
        cout << "0. Keluar\n";
        cout << "Pilih opsi: ";

        cin >> pilihan;
        cin.ignore();

        switch (pilihan) {
            case 1:
                tambahDataMenu();
                break;
            case 2:
                lihatMenu();
                break;
            case 3:
                catatPesanan();
                break;
            case 4:
                ubahStatusMenu();
                break;
            case 5:
                hapusMenu();
                break;
            case 6:
                undoAksi();
                break;
            case 7:
                tampilkanAntrian();
                break;
            case 0:
                cout << "Terima kasih telah menggunakan program ini. Semangat!\n";
                break;
            default:
                cout << "Pilihan tidak valid. Silakan coba lagi.\n";
        }

    } while (pilihan != 0);

    return 0;
}
