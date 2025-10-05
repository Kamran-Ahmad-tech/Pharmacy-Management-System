#include <iostream>
#include <fstream>
#include <string>
using namespace std;

const int MAX_MEDICINES = 100;
const string FILE_NAME = "medicines.txt";

// Base Class: Medicine
class Medicine {
protected:
    string name, description, category, supplier, expiryDate;
    double price;
    int quantity;

public:
    Medicine() : price(0), quantity(0) {}

    void input() {
        cin.ignore(); // Clear newline from previous input
        cout << "Enter Medicine Name: ";
        getline(cin, name);
        cout << "Enter Description: ";
        getline(cin, description);
        cout << "Enter Category: ";
        getline(cin, category);
        cout << "Enter Price: ";
        cin >> price;
        cout << "Enter Quantity: ";
        cin >> quantity;
        cin.ignore();
        cout << "Enter Supplier Name: ";
        getline(cin, supplier);
        cout << "Enter Expiry Date: ";
        getline(cin, expiryDate);
    }

    void display() const {
        cout << "Name: " << name << "\nDescription: " << description
             << "\nCategory: " << category << "\nPrice: " << price
             << "\nQuantity: " << quantity << "\nSupplier: " << supplier
             << "\nExpiry Date: " << expiryDate << "\n";
    }

    string getName() const { return name; }
    string getDescription() const { return description; }
    double getPrice() const { return price; }

    friend istream& operator>>(istream& in, Medicine& m);
    friend ostream& operator<<(ostream& out, const Medicine& m);
};

istream& operator>>(istream& in, Medicine& m) {
    in >> ws;
    getline(in, m.name);
    getline(in, m.description);
    getline(in, m.category);
    in >> m.price >> m.quantity;
    in.ignore();
    getline(in, m.supplier);
    getline(in, m.expiryDate);
    return in;
}

ostream& operator<<(ostream& out, const Medicine& m) {
    out << m.name << "\n" << m.description << "\n" << m.category << "\n"
        << m.price << " " << m.quantity << "\n"
        << m.supplier << "\n" << m.expiryDate << "\n";
    return out;
}

// Base class for all actions (for polymorphism)
class Operation {
public:
    virtual void execute(Medicine meds[], int& count) = 0;
    virtual ~Operation() {}
};

// Derived classes
class AddMedicine : public Operation {
public:
    void execute(Medicine meds[], int& count) override {
        if (count >= MAX_MEDICINES) {
            cout << "Medicine storage full!\n";
            return;
        }
        meds[count].input();
        count++;
        cout << "Medicine added successfully!\n";
    }
};

class EditMedicine : public Operation {
public:
    void execute(Medicine meds[], int& count) override {
        string name;
        cout << "Enter name to edit: ";
        cin >> ws;
        getline(cin, name);
        for (int i = 0; i < count; i++) {
            if (meds[i].getName() == name) {
                cout << "Editing medicine:\n";
                meds[i].input();
                cout << "Medicine updated.\n";
                return;
            }
        }
        cout << "Medicine not found.\n";
    }
};

class SearchMedicine : public Operation {
public:
    void execute(Medicine meds[], int& count) override {
        string name;
        cout << "Enter name to search: ";
        cin >> ws;
        getline(cin, name);
        for (int i = 0; i < count; i++) {
            if (meds[i].getName() == name) {
                cout << "Medicine found:\n";
                meds[i].display();
                return;
            }
        }
        cout << "Medicine not found.\n";
    }
};

class DeleteMedicine : public Operation {
public:
    void execute(Medicine meds[], int& count) override {
        string name;
        cout << "Enter name to delete: ";
        cin >> ws;
        getline(cin, name);
        for (int i = 0; i < count; i++) {
            if (meds[i].getName() == name) {
                for (int j = i; j < count - 1; j++) {
                    meds[j] = meds[j + 1];
                }
                count--;
                cout << "Medicine deleted.\n";
                return;
            }
        }
        cout << "Medicine not found.\n";
    }
};

class DisplayAll : public Operation {
public:
    void execute(Medicine meds[], int& count) override {
        if (count == 0) {
            cout << "No medicines available.\n";
            return;
        }
        for (int i = 0; i < count; i++) {
            cout << "\nMedicine " << i + 1 << ":\n";
            meds[i].display();
        }
    }
};

// File handling with exception safety
class FileManager {
public:
    static void load(Medicine meds[], int& count) {
        try {
            ifstream file(FILE_NAME);
            if (!file) throw "No previous data found.";

            file >> count;
            file.ignore();
            for (int i = 0; i < count; i++) {
                file >> meds[i];
            }
            file.close();
        } catch (const char* msg) {
            cout << msg << "\n";
            count = 0;
        }
    }

    static void save(Medicine meds[], int count) {
        try {
            ofstream file(FILE_NAME);
            if (!file) throw "Error writing to file!";

            file << count << "\n";
            for (int i = 0; i < count; i++) {
                file << meds[i];
            }
            file.close();
        } catch (const char* msg) {
            cout << msg << "\n";
        }
    }
};

int main() {
    Medicine meds[MAX_MEDICINES];
    int count = 0;

    FileManager::load(meds, count);

    Operation* ops[5] = {
        new AddMedicine(),
        new EditMedicine(),
        new SearchMedicine(),
        new DeleteMedicine(),
        new DisplayAll()
    };

    int choice;
    do {
        cout << "\n=== Medicine Management System ===\n";
        cout << "1. Add Medicine\n2. Edit Medicine\n3. Search Medicine\n";
        cout << "4. Delete Medicine\n5. Display All\n6. Exit\n";
        cout << "Enter your choice: ";

        try {
            if (!(cin >> choice)) {
                throw "Invalid input. Please enter a number.";
            }

            if (choice >= 1 && choice <= 5) {
                ops[choice - 1]->execute(meds, count);
                FileManager::save(meds, count);
            } else if (choice != 6) {
                cout << "Invalid choice.\n";
            }
        } catch (const char* msg) {
            cout << msg << "\n";
            cin.clear();
            cin.ignore(1000, '\n');
            choice = 0;
        }

    } while (choice != 6);

    for (int i = 0; i < 5; i++) {
        delete ops[i];
    }

    cout << "Program exited successfully.\n";
    return 0;
}