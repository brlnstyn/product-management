#include <iostream>
#include <stack>
#include <queue>
#include <vector>
#include <limits>

using namespace std;

// --------------------- STRUKTUR DATA ---------------------

struct Product {
    int id;
    string name;
    string category;
    double price;
    int stock;
};

struct CartNode {
    Product product;
    int quantity;
    CartNode *next;

    CartNode(Product p, int qty) : product(p), quantity(qty), next(nullptr) {
    }
};

struct CategoryNode {
    string name;
    vector<CategoryNode *> children;

    CategoryNode(const string &name) : name(name) {
    }
};

// --------------------- DATA GLOBAL ---------------------

Product productList[] = {
    {1, "Toast", "Breakfast", 25000, 10},
    {2, "Salad Buah", "Breakfast", 25000, 5},
    {3, "Ketoprak", "Lunch", 15000, 20},
    {4, "Nasi Goreng", "Dinner", 15000, 2},
    {5, "Steak", "Dinner", 75000, 1},
    {6, "Americano", "Coffee", 20000, 25},
    {7, "Latte", "Coffee", 25000, 10},
    {8, "Jasmine Tea", "Tea", 25000, 10},
};


// --------------------- KATEGORI (TREE) ---------------------

CategoryNode *buildCategoryTree() {
    CategoryNode *root = new CategoryNode("All Category");

    CategoryNode *food = new CategoryNode("Food");
    CategoryNode *drink = new CategoryNode("Drink");

    CategoryNode *breakfast = new CategoryNode("Breakfast");
    CategoryNode *lunch = new CategoryNode("Lunch");
    CategoryNode *dinner = new CategoryNode("Dinner");
    CategoryNode *coffee = new CategoryNode("Coffee");
    CategoryNode *tea = new CategoryNode("Tea");


    food->children.push_back(breakfast);
    food->children.push_back(lunch);
    food->children.push_back(dinner);

    drink->children.push_back(coffee);
    drink->children.push_back(tea);

    root->children.push_back(food);
    root->children.push_back(drink);

    return root;
}

void printCategoryTree(CategoryNode *node, int depth = 0) {
    if (!node) return;
    for (int i = 0; i < depth; ++i) cout << "  ";
    cout << "- " << node->name << "\n";
    for (CategoryNode *child: node->children) {
        printCategoryTree(child, depth + 1);
    }
}

void deleteTree(CategoryNode *node) {
    for (CategoryNode *child: node->children) {
        deleteTree(child);
    }
    delete node;
}

// --------------------- KERANJANG (LINKED LIST) ---------------------

Product findProductById(int id) {
    for (const Product &p: productList) {
        if (p.id == id) return p;
    }
    return {-1, "Product not found!", "", 0};
}

void searchProductByCategory(const string &category) {
    cout << "Product in category '" << category << "':\n";
    bool found = false;
    for (const Product &p: productList) {
        if (p.category == category) {
            cout << "- ID " << p.id << ": " << p.name << " (Rp" << p.price << ")\n";
            found = true;
        }
    }
    if (!found) cout << "There are no product in this category.\n";
}

CartNode *cartHead = nullptr;
stack<CartNode *> undoStack;
void addToCart(int productId, int qty) {
    Product p = findProductById(productId);
    if (p.id == -1) {
        cout << "Product not found.\n";
        return;
    }

    CartNode *node = new CartNode(p, qty);
    node->next = cartHead;
    cartHead = node;

    undoStack.push(node);
    cout << "Product '" << p.name << "' added to cart.\n";
}

void printProduct() {
    cout << "\nList Products:\n";
    for (const Product &p: productList) {
        cout << "- ID " << p.id << ": " << p.name << ": " << p.category << " (Rp" << p.price << ")  \n";
    }
}

void printCart() {
    if (!cartHead) {
        cout << "Cart is empty.\n";
        return;
    }

    cout << "\nCart contents:\n";
    CartNode *temp = cartHead;
    while (temp) {
        cout << "- " << temp->product.name << " x" << temp->quantity << " (Rp" << temp->product.price << ")\n";
        temp = temp->next;
    }
}


void undoLastAdd() {
    if (undoStack.empty()) {
        cout << "There is no action.\n";
        return;
    }

    CartNode *toUndo = undoStack.top();
    undoStack.pop();

    if (cartHead == toUndo) {
        cartHead = cartHead->next;
    } else {
        CartNode *prev = cartHead;
        while (prev->next && prev->next != toUndo) {
            prev = prev->next;
        }
        if (prev->next) {
            prev->next = toUndo->next;
        }
    }

    delete toUndo;
    cout << "Last action is undo.\n";
}

// --------------------- CHECKOUT (QUEUE) ---------------------

queue<string> transactionQueue;
void transaction(const string &username) {
    if (!cartHead) {
        cout << "Cart is empty. Can not make transaction.\n";
        return;
    }

    transactionQueue.push(username);
    cout << "\nCustomer '" << username << "' add to transaction queue.\n";

    cout << "Processing transaction for " << username << "...\n";
    double total = 0;
    CartNode *temp = cartHead;
    while (temp) {
        total += temp->product.price * temp->quantity;
        temp = temp->next;
    }

    cout << "Total payment: Rp" << total << "\n";

    while (cartHead) {
        CartNode *del = cartHead;
        cartHead = cartHead->next;
        delete del;
    }

    while (!undoStack.empty()) undoStack.pop();

    cout << "Transaction success. Cart emptied.\n";
}

void printTransactionQueue() {
    if (transactionQueue.empty()) {
        cout << "There is no transaction queue.\n";
        return;
    }

    cout << "\nTransaction queue:\n";
    queue<string> temp = transactionQueue;
    while (!temp.empty()) {
        cout << "- " << temp.front() << "\n";
        temp.pop();
    }
}

// --------------------- MENU ---------------------

void showMenu() {
    cout << "\nWELCOME TO OUR MERCHANT!\n Choose Menu: \n";
    cout << "1. Show Category Product\n";
    cout << "2. Show Product\n";
    cout << "3. Search Product by Category\n";
    cout << "4. Add Product to Cart\n";
    cout << "5. Show Cart\n";
    cout << "6. Undo Last Product Addition\n";
    cout << "7. Transaction\n";
    cout << "8. Show Transaction Queue\n";
    cout << "0. Close\n";
    cout << "Choose Menu: ";
}

void clearCin() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// --------------------- MAIN ---------------------

int main() {
    CategoryNode *rootCategory = buildCategoryTree();
    int choice;

    do {
        showMenu();
        cin >> choice;
        clearCin();

        switch (choice) {
            case 1:
                printCategoryTree(rootCategory);
                break;
            case 2: {
                printProduct();
                break;
            }
            case 3: {
                string cat;
                cout << "Category: ";
                getline(cin, cat);
                searchProductByCategory(cat);
                break;
            }
            case 4: {
                int id, qty;
                cout << "Product Id: ";
                cin >> id;
                cout << "Quantity: ";
                cin >> qty;
                clearCin();
                addToCart(id, qty);
                break;
            }
            case 5:
                printCart();
                break;
            case 6:
                undoLastAdd();
                break;
            case 7: {
                string user;
                cout << "Customer Name: ";
                getline(cin, user);
                transaction(user);
                break;
            }
            case 8:
                printTransactionQueue();
                break;
            case 0:
                cout << "Thank you for using our service! Happy Shopping!\n";
                break;
            default:
                cout << "Menu is not valid.\n";
        }
    } while (choice != 0);

    deleteTree(rootCategory);
    return 0;
}
