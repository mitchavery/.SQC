#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlenv.h>

const char DB_NAME_KEY[16] = "db2.name";
const char DB_USER_KEY[16] = "db2.username";
const char DB_PASS_KEY[16] = "db2.password";
const char BATCH_KEY[16] = "p3.batch.input";

struct sqlca sqlca;

EXEC SQL BEGIN DECLARE SECTION;
char db_name[20];
char db_user[20];
char db_pass[20];

int cus_id;
char cus_name[15];
char cus_gender;
int cus_age;
int cus_pin;
int acc_num;
int acc_id;
int acc_bal;
char acc_type;
char acc_status;

int src_cid;
int dest_cid;
int trans_amt;
int a_total;
int b_min;
int b_max;
int b_avg;
EXEC SQL END DECLARE SECTION;

/**
 * Create a new customer.
 * @param name customer name
 * @param gender customer gender
 * @param age customer age
 * @param pin customer pin
 */
static void newCustomer(char *name, char *gender, char *age, char *pin)
{
  printf(":: CREATE NEW CUSTOMER - RUNNING!\n");
  struct sqlca sqlca;
  strcpy(cus_name, name);
  cus_age = atoi(age);
  cus_pin = atoi(pin);
  cus_gender = *gender;
  if (cus_age < 0 || cus_pin < 0)
  {
    printf("Invalid information provided");
    return;
  }
  EXEC SQL INSERT INTO p3.customer(name, gender, age, pin) values(
      : cus_name,
      : cus_gender,
      : cus_age,
      : cus_pin);
  EXEC SQL select id into : cus_id from p3.Customer where name = : cus_name and gender = : cus_gender and age = : cus_age;
  if (sqlca.sqlcode == 100)
  {
    printf("\n  Data not found.\n");
  }
  else if (SQLCODE < 0)
  {
    printf("Insert Error:  SQLCODE");
  }
  else
    {
      printf(":: ID CREATED IS: %d \n", cus_id);
      printf(":: CREATE NEW CUSTOMER - SUCCESS!\n");
    }
}

/**
 * Open a new account.
 * @param id customer id
 * @param type type of account
 * @param amount initial deposit amount
 */
static void openAccount(char *id, char *type, char *amount)
{
  printf(":: OPEN ACCOUNT - RUNNING\n");
  struct sqlca sqlca;
  acc_id = atoi(id);
  acc_bal = atoi(amount);
  acc_type = *type;
  if (acc_bal < 0)
  {
    printf("Invalid input");
    return;
  } 
  EXEC SQL INSERT INTO P3.account(ID, Balance, Type, Status) values(
      : acc_id,
      : acc_bal,
      : acc_type, 'A');
  EXEC SQL select Number into : acc_num from p3.account where id = : acc_id and type = : acc_type;
  if (sqlca.sqlcode == 100)
  {
    printf("\n  Data not found.\n");
  }
  else if (SQLCODE < 0)
  {
    printf("Insert Error:  SQLCODE");
  }
  else
    {
      printf(":: ACCOUNT NUMBER CREATED IS: %d \n", acc_num);
      printf(":: OPEN ACCOUNT - SUCCESS\n");
    }
}

/**
 * Close an account.
 * @param accNum account number
 */
static void closeAccount(char *accNum)
{
  printf(":: CLOSE ACCOUNT - RUNNING\n");
  struct sqlca sqlca;
  acc_num = atoi(accNum);
  EXEC SQL Update P3.account set status = 'I', balance = 0 where number = : acc_num;
  if (sqlca.sqlcode == 100)
  {
    printf("\n  Data not found.\n");
  }
  else if (SQLCODE < 0)
  {
    printf("Update Error:  SQLCODE");
  }
  else
    printf(":: CLOSE ACCOUNT - SUCCESS\n");
}

/**
 * Deposit into an account.
 * @param accNum account number
 * @param amount deposit amount
 */
static void deposit(char *accNum, char *amount)
{
  printf(":: DEPOSIT - RUNNING\n");
  struct sqlca sqlca;
  acc_num = atoi(accNum);
  acc_bal = atoi(amount);
  if (acc_bal < 0)
  {
    printf("Invalid input");
    return;
  }
  EXEC SQL Update P3.account set Balance = Balance + : acc_bal where Number = : acc_num and status = 'A';
  if (sqlca.sqlcode == 100)
  {
    printf("\n  Data not found.\n");
  }
  else if (SQLCODE < 0)
  {
    printf("Update Error:  SQLCODE");
  }
  else
    printf(":: DEPOSIT - SUCCESS\n");
}

/**
 * Withdraw from an account.
 * @param accNum account number
 * @param amount withdraw amount
 */
static void withdraw(char *accNum, char *amount)
{
  printf(":: WITHDRAW - RUNNING\n");
  struct sqlca sqlca;
  acc_num = atoi(accNum);
  acc_bal = atoi(amount);
  if (acc_bal < 0)
  {
    printf("Invalid input");
    return;
  }
  EXEC SQL Update P3.account set Balance = Balance - : acc_bal where Number = : acc_num and status = 'A';
  if (sqlca.sqlcode == 100)
  {
    printf("\n  Data not found.\n");
  }
  else if (SQLCODE < 0)
  {
    printf("Update Error:  SQLCODE");
  }
  else
    printf(":: WITHDRAW - SUCCESS\n");
}

/**
 * Transfer amount from source account to destination account. 
 * @param srcAccNum source account number
 * @param destAccNum destination account number
 * @param amount transfer amount
 */
static void transfer(char *srcAccNum, char *destAccNum, char *amount)
{
  printf(":: TRANSFER - RUNNING\n");
  struct sqlca sqlca;
  src_cid = atoi(srcAccNum);
  dest_cid = atoi(destAccNum);
  acc_bal = atoi(amount);
  if (acc_bal < 0)
  {
    printf("Invalid input");
    return;
  }
  EXEC SQL Update p3.account set Balance = Balance - : acc_bal where Number = : src_cid and status = 'A';
  EXEC SQL Update p3.account set Balance = Balance + : acc_bal where Number = : dest_cid and status = 'A';
  if (sqlca.sqlcode == 100)
  {
    printf("\n  Data not found.\n");
  }
  else if (SQLCODE < 0)
  {
    printf("Update Error:  SQLCODE");
  }
  else
    printf(":: TRANSFER - SUCCESS\n");
}

/**
 * Display account summary.
 * @param cusID customer ID
 */
static void accountSummary(char *cusID)
{
  printf(":: ACCOUNT SUMMARY - RUNNING\n");
  struct sqlca sqlca;
  cus_id = atoi(cusID);
  printf("\n%-11s %-11s\n", "NUMBER", "BALANCE");
  printf("----------- -----------\n");
  EXEC SQL DECLARE c1 CURSOR FOR
      Select Number,
      Balance from p3.account where ID = : cus_id and status = 'A';
  EXEC SQL OPEN c1;
  EXEC SQL FETCH c1 INTO : acc_num, : acc_bal;
  if (sqlca.sqlcode == 100)
  {
    printf("\n  Data not found.\n");
  }
  else if (SQLCODE < 0)
  {
    printf("Select Error:  SQLCODE");
  }
  else {
    while (sqlca.sqlcode != 100)
    {
      printf("%11d %11d\n", acc_num, acc_bal);
      EXEC SQL FETCH c1 INTO : acc_num, : acc_bal;
    }
      EXEC SQL CLOSE c1;
      /* EMB_SQL_CHECK("Cursor -- Close"); */
      printf(":: ACCOUNT SUMMARY - SUCCESS\n");
    }
}

/**
 * Display Report A - Customer Information with Total Balance in Decreasing Order.
 */
static void reportA()
{
  printf(":: REPORT A - RUNNING\n");
  struct sqlca sqlca;
  printf("\n%-10s %-15s %-10s %-10s %-10s %-10s\n", "ID", "NAME", "Gender", "AGE", "Pin", "Total");
  printf("---------- --------------- ---------- ---------- ---------- ----------\n");
  EXEC SQL DECLARE c2 CURSOR FOR
      select ID,
      Name, Gender, Age, Pin, TOTAL from p3.customer join p3.total_balance on accountID = p3.customer.id order by Total Desc;
  EXEC SQL OPEN c2;
  EXEC SQL FETCH c2 INTO : cus_id, : cus_name, : cus_gender, : cus_age, : cus_pin, : a_total;
  if (sqlca.sqlcode == 100)
  {
    printf("\n  Data not found.\n");
  }
  else if (SQLCODE < 0)
  {
    printf("Select Error:  SQLCODE");
  }
  else {
    while (sqlca.sqlcode != 100)
    {
      printf("%10d %15s %10c %10d %10d %10d\n", cus_id, cus_name, cus_gender, cus_age, cus_pin, a_total);
      EXEC SQL FETCH c2 INTO : cus_id, : cus_name, : cus_gender, : cus_age, : cus_pin, : a_total;
    }
    EXEC SQL CLOSE c2;
    /* EMB_SQL_CHECK("Cursor -- Close"); */
    printf(":: REPORT A - SUCCESS\n");
  }
}
/**
 * Display Report B - Customer Information with Total Balance in Decreasing Order.
 * @param min minimum age
 * @param max maximum age
 */
static void reportB(char *min, char *max)
{
  printf(":: REPORT B - RUNNING\n");
  struct sqlca sqlca;
  b_min = atoi(min);
  b_max = atoi(max);
  if (b_min > b_max)
  {
    printf("Invalid Information");
    return;
  }
  printf("\n%-15s %-19s\n", "  AVERAGE AGE  ", "  AVERAGE BALANCE  ");
  printf("--------------- -------------------\n");
  EXEC SQL DECLARE c3 CURSOR FOR
      Select AVG(age),
      AVG(balance) from p3.customer as C, p3.account as A where C.age >= : b_min AND C.age <= : b_max and A.status = 'A';
  EXEC SQL OPEN c3;
  EXEC SQL FETCH c3 INTO : cus_age, : b_avg;
  if (sqlca.sqlcode == 100)
  {
    printf("\n  Data not found.\n");
  }
  else if (SQLCODE < 0)
  {
    printf("Select Error:  SQLCODE");
  }
  else {
    while (sqlca.sqlcode != 100)
    {
      printf("%15d %19d\n", cus_age, b_avg);
      EXEC SQL FETCH c3 INTO : cus_age, : b_avg;
    }
    EXEC SQL CLOSE c3;
    /* EMB_SQL_CHECK("Cursor -- Close"); */
    printf(":: REPORT B - SUCCESS\n");
  }
}

/**
 * Retrieve database connection info from properties file.
 * @param filename name of properties file
 */
static void init(char filename[])
{
  // open file
  FILE *fp = fopen(filename, "r");
  // print error if file cannot be opened
  if (fp == NULL)
  {
    printf("Cannot open properties file %s\n", filename);
  }
  else
  {
    char *cur_line = (char *)malloc(256);
    // read each line
    while (fscanf(fp, "%s", cur_line) != EOF)
    {
      char *line_copy = (char *)malloc(256);
      char *token;
      // copy current line
      strcpy(line_copy, cur_line);
      // tokenize line and save connection values
      while ((token = strsep(&line_copy, "=")) != NULL)
      {
        if (strcmp(token, DB_NAME_KEY) == 0)
        {
          token = strsep(&line_copy, "=");
          strcpy(db_name, token);
        }
        else if (strcmp(token, DB_USER_KEY) == 0)
        {
          token = strsep(&line_copy, "=");
          strcpy(db_user, token);
        }
        else if (strcmp(token, DB_PASS_KEY) == 0)
        {
          token = strsep(&line_copy, "=");
          strcpy(db_pass, token);
        }
        else
        {
          return;
        }
      }
    }
  }
  // close file
  fclose(fp);
}

/**
 * Open database connection.
 */
static void open_db()
{
  EXEC SQL CONNECT TO : db_name USER : db_user USING : db_pass;
}

/**
 * Close database connection.
 */
static void close_db()
{
  EXEC SQL CONNECT RESET;
}

/**
 * Test database connection.
 */
static void test_connection()
{
  printf(":: TEST - CONNECTING TO DATABASE\n");
  open_db();
  // check returned sqlcode
  if (sqlca.sqlcode == 0)
  {
    printf(":: TEST - SUCCESSFULLY CONNECTED TO DATABASE\n");
    close_db();
    printf(":: TEST - DISCONNECTED FROM DATABASE\n");
  }
  else
  {
    printf(":: TEST - FAILED CONNECTED TO DATABASE\n");
    printf(":::: ERROR SQLCODE %i\n", sqlca.sqlcode);
  }
}

/**
 * Execute function from batch input.
 * @param arr array of strings
 * @param size length of p array
 */
void batch_execute(char **arr, int size)
{
  if (size < 0)
  {
    printf("Invalid parameter count");
  }
  else
  {
    printf("\n");
    if (strcmp(arr[0], "#newCustomer") == 0)
    {
      newCustomer(arr[1], arr[2], arr[3], arr[4]);
    }
    else if (strcmp(arr[0], "#openAccount") == 0)
    {
      openAccount(arr[1], arr[2], arr[3]);
    }
    else if (strcmp(arr[0], "#closeAccount") == 0)
    {
      closeAccount(arr[1]);
    }
    else if (strcmp(arr[0], "#deposit") == 0)
    {
      deposit(arr[1], arr[2]);
    }
    else if (strcmp(arr[0], "#withdraw") == 0)
    {
      withdraw(arr[1], arr[2]);
    }
    else if (strcmp(arr[0], "#transfer") == 0)
    {
      transfer(arr[1], arr[2], arr[3]);
    }
    else if (strcmp(arr[0], "#accountSummary") == 0)
    {
      accountSummary(arr[1]);
    }
    else if (strcmp(arr[0], "#reportA") == 0)
    {
      reportA();
    }
    else if (strcmp(arr[0], "#reportB") == 0)
    {
      reportB(arr[1], arr[2]);
    }
    else
    {
      printf("Could not find function to batch_execute: %s", arr[0]);
    }
    printf("\n");
  }
}

/**
 * Run batch input given filename.
 * @param filename name of properties file
 */
static void batch_run(char filename[])
{
  // open file
  FILE *fp = fopen(filename, "r");
  // print error if file cannot be opened
  if (fp == NULL)
  {
    printf("Cannot open properties file %s\n", filename);
  }
  else
  {
    int batch_mode = 0;
    char *cur_line = (char *)malloc(256);
    // read each line
    while (fscanf(fp, "%s", cur_line) != EOF)
    {
      char *line_copy = (char *)malloc(256);
      char *token;
      // copy current line
      strcpy(line_copy, cur_line);
      if (batch_mode == 0)
      {
        // tokenize line and find batch key
        while ((token = strsep(&line_copy, "=")) != NULL)
        {
          if (strcmp(token, BATCH_KEY) == 0)
          {
            batch_mode = 1;
          }
          else
          {
            break;
          }
        }
      }
      else if (batch_mode == 1)
      {
        char *token;
        char **token_arr = (char **)malloc(sizeof(char *) * 1);
        int token_cnt = 0;
        // get each token and save to array
        while ((token = strsep(&line_copy, ",")) != NULL)
        {
          token_arr = (char **)realloc(token_arr, (token_cnt + 1) * sizeof(char *));
          token_arr[token_cnt] = (char *)malloc(strlen(token) + 1);
          strcpy(token_arr[token_cnt], token);
          token_cnt++;
        }
        // run function
        batch_execute(token_arr, token_cnt);
      }
    }
  }
  // close file
  fclose(fp);
}

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    printf("Need properties file\n");
  }
  else
  {
    init(argv[1]);
    test_connection();

    open_db();
    batch_run(argv[1]);
    close_db();
  }
  return 0;
}