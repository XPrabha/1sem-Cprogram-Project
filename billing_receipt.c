#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define MAX_ITEMS     20
#define MAX_NAME_LEN  50
#define TAX_RATE      0.10f    
#define DISCOUNT_RATE 0.05f    
#define STORE_NAME    "KG SMART STORE"
#define STORE_ADDR    "Saravanampatti, Coimbatore - 641 035"
#define STORE_PHONE   "+91 98765 43210"
typedef struct {
    char  name[MAX_NAME_LEN];
    float price;
    int   quantity;
    float total;
} Item;
typedef struct {
    char  customer_name[MAX_NAME_LEN];
    char  customer_phone[15];
    Item  items[MAX_ITEMS];
    int   item_count;
    float subtotal;
    float discount;
    float tax;
    float grand_total;
    int   payment_type;    
    float amount_paid;
    float change_due;
} Bill;
void  print_header(void);
void  print_divider(char ch, int len);
void  get_customer_details(Bill *b);
void  add_items(Bill *b);
void  calculate_bill(Bill *b);
void  display_summary(const Bill *b);
int   process_payment(Bill *b);
void  print_receipt(const Bill *b);
void  save_receipt(const Bill *b);
char *current_datetime(void);
void  clear_screen(void);
void  pause(void);
int main(void)
{
    Bill bill;
    memset(&bill, 0, sizeof(Bill));
    int choice;
    clear_screen();
    print_header();
    printf("\n  Welcome to the Billing & Receipt System\n");
    print_divider('=', 50);
    printf("\n  1. New Bill\n");
    printf("  2. Exit\n");
    printf("\n  Enter choice: ");
    scanf("%d", &choice);
    getchar();  
    if (choice != 1) {
        printf("\n  Thank you! Goodbye.\n\n");
        return 0;
    }
    get_customer_details(&bill);
    add_items(&bill);
    if (bill.item_count == 0) {
        printf("\n  No items added. Exiting.\n");
        return 0;
    }
    calculate_bill(&bill);
    display_summary(&bill);
    if (!process_payment(&bill)) {
        printf("\n  Payment failed. Bill cancelled.\n");
        return 0;
    }
    print_receipt(&bill);
    save_receipt(&bill);
    printf("\n  Receipt saved to  receipt.txt\n");
    printf("  Thank you for shopping at %s!\n\n", STORE_NAME);
    return 0;
}
void print_header(void)
{
    print_divider('*', 50);
    printf("*%*s*\n", 48, "");
    printf("*   %-46s*\n", STORE_NAME);
    printf("*   %-46s*\n", STORE_ADDR);
    printf("*   %-46s*\n", STORE_PHONE);
    printf("*%*s*\n", 48, "");
    print_divider('*', 50);
}
void print_divider(char ch, int len)
{
    for (int i = 0; i < len; i++) putchar(ch);
    putchar('\n');
}
void get_customer_details(Bill *b)
{
    printf("\n--- CUSTOMER DETAILS ---\n");
    printf("  Customer Name  : ");
    fgets(b->customer_name, MAX_NAME_LEN, stdin);
    b->customer_name[strcspn(b->customer_name, "\n")] = '\0';
    printf("  Mobile Number  : ");
    fgets(b->customer_phone, 15, stdin);
    b->customer_phone[strcspn(b->customer_phone, "\n")] = '\0';
}
void add_items(Bill *b)
{
    int  more = 1;
    char again;
    printf("\n--- ADD ITEMS ---\n");
    while (more && b->item_count < MAX_ITEMS) {
        Item *it = &b->items[b->item_count];
        printf("\n  Item %d\n", b->item_count + 1);
        printf("  Name     : ");
        fgets(it->name, MAX_NAME_LEN, stdin);
        it->name[strcspn(it->name, "\n")] = '\0';
        printf("  Price(Rs): ");
        scanf("%f", &it->price);
        printf("  Quantity : ");
        scanf("%d", &it->quantity);
        getchar();  
        if (it->price <= 0 || it->quantity <= 0) {
            printf("  Invalid price/quantity. Item skipped.\n");
        } else {
            it->total = it->price * it->quantity;
            b->item_count++;
            printf("  Item added! (Rs %.2f x %d = Rs %.2f)\n",
                   it->price, it->quantity, it->total);
        }
        if (b->item_count < MAX_ITEMS) {
            printf("\n  Add another item? (y/n): ");
            scanf(" %c", &again);
            getchar();
            if (again != 'y' && again != 'Y') more = 0;
        } else {
            printf("  Maximum items reached.\n");
            more = 0;
        }
    }
}
void calculate_bill(Bill *b)
{
    b->subtotal = 0.0f;
    for (int i = 0; i < b->item_count; i++)
        b->subtotal += b->items[i].total;
    if (b->subtotal > 200.0f) {
        b->discount = 0.0f;
        b->tax      = b->subtotal * TAX_RATE;
    } else {
        b->discount = b->subtotal * DISCOUNT_RATE;
        b->tax      = 0.0f;
        printf("\n  ** Small Purchase Discount of 5%% Applied! **\n");
    }
    b->grand_total = b->subtotal - b->discount + b->tax;
}
void display_summary(const Bill *b)
{
    printf("\n--- BILL SUMMARY ---\n");
    printf("  %-25s %6s %8s %10s\n",
           "Item", "Qty", "Price", "Total");
    print_divider('-', 55);
    for (int i = 0; i < b->item_count; i++) {
        printf("  %-25s %6d %8.2f %10.2f\n",
               b->items[i].name,
               b->items[i].quantity,
               b->items[i].price,
               b->items[i].total);
    }
    print_divider('-', 55);
    printf("  %-40s Rs %8.2f\n", "Subtotal",    b->subtotal);
    if (b->discount > 0)
        printf("  %-40s Rs %8.2f\n", "Discount (5%)", b->discount);
    if (b->tax > 0)
        printf("  %-40s Rs %8.2f\n", "GST (10%)",    b->tax);
    print_divider('=', 55);
    printf("  %-40s Rs %8.2f\n", "GRAND TOTAL", b->grand_total);
    print_divider('=', 55);
}
int process_payment(Bill *b)
{
    printf("\n--- PAYMENT ---\n");
    printf("  1. Cash\n  2. Card\n  3. UPI\n");
    printf("  Choose payment method: ");
    scanf("%d", &b->payment_type);
    getchar();
    if (b->payment_type < 1 || b->payment_type > 3) {
        printf("  Invalid payment method.\n");
        return 0;
    }
    if (b->payment_type == 1) {   
        printf("  Amount to pay : Rs %.2f\n", b->grand_total);
        printf("  Cash received : Rs ");
        scanf("%f", &b->amount_paid);
        getchar();
        if (b->amount_paid < b->grand_total) {
            printf("  Insufficient amount! Need Rs %.2f more.\n",
                   b->grand_total - b->amount_paid);
            return 0;
        }
        b->change_due = b->amount_paid - b->grand_total;
        printf("  Change due    : Rs %.2f\n", b->change_due);
    } else if (b->payment_type == 2) {   
        char last4[5];
        printf("  Enter last 4 digits of card: ");
        fgets(last4, 5, stdin);
        getchar();
        printf("  Card payment of Rs %.2f processed successfully.\n",
               b->grand_total);
        b->amount_paid = b->grand_total;
    } else {   
        char upi_id[50];
        printf("  Enter UPI ID: ");
        fgets(upi_id, 50, stdin);
        upi_id[strcspn(upi_id, "\n")] = '\0';
        printf("  UPI payment of Rs %.2f to %s successful.\n",
               b->grand_total, upi_id);
        b->amount_paid = b->grand_total;
    }
    printf("\n  ** Payment Successful! **\n");
    return 1;
}
void print_receipt(const Bill *b)
{
    const char *pay_methods[] = {"", "Cash", "Card", "UPI"};
    printf("\n\n");
    print_divider('=', 50);
    printf("          PAYMENT RECEIPT\n");
    print_divider('=', 50);
    printf("  Store  : %s\n", STORE_NAME);
    printf("  Date   : %s\n", current_datetime());
    printf("  Name   : %s\n", b->customer_name);
    printf("  Phone  : %s\n", b->customer_phone);
    print_divider('-', 50);
    printf("  %-22s %5s %7s %9s\n",
           "Item", "Qty", "Price", "Amount");
    print_divider('-', 50);
    for (int i = 0; i < b->item_count; i++) {
        printf("  %-22s %5d %7.2f %9.2f\n",
               b->items[i].name,
               b->items[i].quantity,
               b->items[i].price,
               b->items[i].total);
    }
    print_divider('-', 50);
    printf("  %-38s %7.2f\n", "Subtotal (Rs)",    b->subtotal);
    if (b->discount > 0)
        printf("  %-38s %7.2f\n", "Discount 5% (Rs)", b->discount);
    if (b->tax > 0)
        printf("  %-38s %7.2f\n", "GST 10% (Rs)",     b->tax);
    print_divider('=', 50);
    printf("  %-38s %7.2f\n", "GRAND TOTAL (Rs)", b->grand_total);
    print_divider('=', 50);
    printf("  Payment Method : %s\n", pay_methods[b->payment_type]);
    printf("  Amount Paid    : Rs %.2f\n", b->amount_paid);
    if (b->change_due > 0)
        printf("  Change Due     : Rs %.2f\n", b->change_due);
    print_divider('-', 50);
    printf("  ** Thank you, %s! Visit again. **\n", b->customer_name);
    print_divider('=', 50);
    printf("\n");
}
void save_receipt(const Bill *b)
{
    const char *pay_methods[] = {"", "Cash", "Card", "UPI"};
    FILE *fp = fopen("receipt.txt", "w");
    if (!fp) return;
    fprintf(fp, "==================================================\n");
    fprintf(fp, "           PAYMENT RECEIPT\n");
    fprintf(fp, "==================================================\n");
    fprintf(fp, "Store  : %s\n", STORE_NAME);
    fprintf(fp, "Date   : %s\n", current_datetime());
    fprintf(fp, "Name   : %s\n", b->customer_name);
    fprintf(fp, "Phone  : %s\n", b->customer_phone);
    fprintf(fp, "--------------------------------------------------\n");
    fprintf(fp, "%-22s %5s %7s %9s\n", "Item","Qty","Price","Amount");
    fprintf(fp, "--------------------------------------------------\n");
    for (int i = 0; i < b->item_count; i++) {
        fprintf(fp, "%-22s %5d %7.2f %9.2f\n",
                b->items[i].name,
                b->items[i].quantity,
                b->items[i].price,
                b->items[i].total);
    }
    fprintf(fp, "--------------------------------------------------\n");
    fprintf(fp, "%-38s %7.2f\n", "Subtotal (Rs)",    b->subtotal);
    if (b->discount > 0)
        fprintf(fp, "%-38s %7.2f\n", "Discount 5% (Rs)", b->discount);
    if (b->tax > 0)
        fprintf(fp, "%-38s %7.2f\n", "GST 10% (Rs)",     b->tax);
    fprintf(fp, "==================================================\n");
    fprintf(fp, "%-38s %7.2f\n", "GRAND TOTAL (Rs)", b->grand_total);
    fprintf(fp, "==================================================\n");
    fprintf(fp, "Payment Method : %s\n", pay_methods[b->payment_type]);
    fprintf(fp, "Amount Paid    : Rs %.2f\n", b->amount_paid);
    if (b->change_due > 0)
        fprintf(fp, "Change Due     : Rs %.2f\n", b->change_due);
    fprintf(fp, "--------------------------------------------------\n");
    fprintf(fp, "Thank you, %s! Visit again.\n", b->customer_name);
    fprintf(fp, "==================================================\n");
    fclose(fp);
}
char *current_datetime(void)
{
    static char buf[30];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buf, sizeof(buf), "%d-%m-%Y  %H:%M:%S", tm_info);
    return buf;
}
void clear_screen(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}