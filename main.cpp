#include <iostream>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <locale>
#include <regex>

using namespace std;

int MIN_BAL = 500;
int max_accno = 33000;
int INVALID_ATTEMPTS = 3;
string DATA_FILE = "csv.txt";
string ANSWERS_FILE = "questions.txt";

string tolower(string &data) {
	string result = "";
	for(auto x: data) {
		if(x >= 'A' && x <= 'Z') result += (x+32);
		else result += x;
	}
	return result;
}
// trim from left
inline std::string &ltrim(std::string &s, const char *t = " \t\n\r\f\v")
{
	s.erase(0, s.find_first_not_of(t));
	return s;
}

// trim from right
inline std::string &rtrim(std::string &s, const char *t = " \t\n\r\f\v")
{
	s.erase(s.find_last_not_of(t) + 1);
	return s;
}

// trim from left & right
inline std::string &trim(std::string &s, const char *t = " \t\n\r\f\v")
{
	return ltrim(rtrim(s, t), t);
}

/**
 * @paramter: database
 * @description: writes to csv the contents in DATA_FILE
 */
void save(unordered_map<int, unordered_map<string, string>> &database)
{
	string line, token;
	ofstream data;
	data.open(DATA_FILE);
	if (!data.is_open())
	{
		throw bad_exception();
	}
	cout << "Saving data..\n";
	try
	{
		for (auto row : database)
		{
			data << to_string(row.first) << ",";
			data << (row.second["name"]) << ",";
			data << (row.second["phone"]) << ",";
			data << (row.second["email"]) << ",";
			data << (row.second["dob"]) << ",";
			data << (row.second["aadhaar"]) << ",";
			data << (row.second["password"]) << ",";
			data << (row.second["amount"]) << "\n";
		}
	}
	catch (exception e)
	{
		cerr << "Fatal error while saving to database\n";
		exit(-1);
	}
	cout << "Saved.\n";
	data.close();
}

/**
 * @paramter: answers
 * @description: writes to csv the contents in ANSWERS_FILE
 */
void saveAnswers(unordered_map<int, unordered_map<string, string>> &answers)
{
	string line, token;
	ofstream data;
	data.open(ANSWERS_FILE);
	if (!data.is_open())
	{
		throw bad_exception();
	}
	cout << "Saving data..\n";
	try
	{
		for (auto row : answers)
		{
			data << to_string(row.first) << ",";
			data << (row.second["ans1"]) << ",";
			data << (row.second["ans2"]) << ",";
			data << (row.second["ans3"]) << ",";
			data << (row.second["ans4"]) << "\n";
		}
	}
	catch (exception e)
	{
		cerr << "Fatal error while saving to database\n";
		exit(-1);
	}
	cout << "Saved.\n";
	data.close();
}

/**
 * @parameter: the variable of type string to be used to store the data in
 * @description: read input and make sure the input is "safe" 
 */
bool readString(string &variable)
{
	int attempts = 0;
	string input;
	while (true)
	{
		try
		{
			getline(cin >> ws, variable);
			trim(variable);
			for (auto x : variable)
			{
				if (x == ',' || x == '\\')
				{
					variable = "";
					throw invalid_argument("");
					break;
				}
			}
			break;
		}
		catch (exception e)
		{
			cout << "\nInvalid input data!\n";
			if (attempts > INVALID_ATTEMPTS)
				exit(0);
		}
	}
	return true;
}


/**
 * @parameter: the variable of type int to be used to store the data in
 * @description: read input and make sure the input is "safe" 
 */
bool readInt(int &variable)
{
	int attempts = 0;
	string input;
	while (true)
	{
		try
		{
			attempts++;
			getline(cin >> ws, input);
			variable = stoi(input, nullptr, 10);
			break;
		}
		catch (exception e)
		{
			cout << "\nInvalid input data!\n";
			if (attempts > INVALID_ATTEMPTS)
				exit(0);
		}
	}
	return true;
}

/**
 * @parameters: account number and database
 * @description: checks if recipient exists, then, transfers the amount from user to recipient 
 */
void transferData(int accno, unordered_map<int, unordered_map<string, string>> &database)
{
	int current_bal = stoi(database[accno]["amount"], nullptr, 10);
	int recipient, amount;
	cout << "\nEnter the account number to which you want to transfer\n";
	readInt(recipient);
	if (database.find(recipient) == database.end())
	{
		cout << "\nRecipient not found\n";
		return;
	}
	cout << "\nEnter the amount you want to transfer\n";
	readInt(amount);
	if (current_bal - amount >= MIN_BAL)
	{			
		
		int prev_rec_amt = stoi(database[recipient]["amount"], nullptr, 10);
		database[recipient]["amount"] = to_string((prev_rec_amt + amount));
	
		int prev_sender_amt = stoi(database[accno]["amount"], nullptr, 10);
		database[accno]["amount"] = to_string((prev_sender_amt - amount));

		save(database);
		return;		
	}
	else
	{
		cout << "\nInsufficient funds\n";
		return;
	}
	
}

/**
 * @parameter: account number and database
 * @description: Withdraws the amount and allows withdrawal only for when remaining amount is >= MIN_BAL
 */
void withdraw(int accno, unordered_map<int, unordered_map<string, string>> &database)
{
	int amt, bal, x;
	bal = stoi(database[accno]["amount"], nullptr, 10);
	cout << "Enter the amount you want to withdraw:\n";
	readInt(amt);
	if (bal - amt >= MIN_BAL)
	{
		cout << "\nAmount is successfully withdraw\n";
		x = bal - amt;
		cout << "Your account balance is: " << x << endl;
		database[accno]["amount"] = to_string(x);
		save(database);
	}
	else
	{
		cout << "\nYou cannot withdraw that amount\n";
	}
}

/**
 * @parameter: account number and database
 * @description: read the amount and credit it in the accno given
 */
void deposit(int accno, unordered_map<int, unordered_map<string, string>> &database)
{
	int amt, bal, x;
	bal = stoi(database[accno]["amount"], nullptr, 10);
	cout << "Enter the amount you want to desposit\n";
	readInt(amt);
	x = bal + amt;
	database[accno]["amount"] = to_string(x);
	save(database);
	cout << "\nYour account balance is " << x << endl;
}

/**
 * @parameters: account number and account details
 * @description: displays the account details along with account number
 */
void display(int accno, unordered_map<string, string> database_row)
{
	cout << "Displaying your details......\n";
	cout << "\nAccount number :" << accno;
	cout << "\nName :" << database_row["name"];
	cout << "\nPhone number :" << database_row["phone"];
	cout << "\nEmail :" << database_row["email"];
	cout << "\nDob :" << database_row["dob"];
	cout << "\nAadhaar number :" << database_row["aadhaar"];
	cout << "\nBalance amount :" << database_row["amount"];
	cout << endl;
}

/**
 * @parameters: account details
 * @description: displays balance amount 
 */
void balance_check(unordered_map<string, string> database_row)
{
	cout << "\nYour balance amount is: " << database_row["amount"] << endl;
}

/**
 * @parameters: account number and database
 * @description: modifies the selected details
 * @options: Phone, Email, Password, Exit
 */
void modify(int accno, unordered_map<int, unordered_map<string, string>> &database)
{
	int ch;
	string new_phone, new_email, new_password, confirm_password, old_password;
	while (1)
	{
		cout << "Please select the option\n";
		cout << "\n1.Phone \n2.Email \n3.Password \n4.Exit\n";
		readInt(ch);
		switch (ch)
		{
		case 1:
			cout << "\nCurrent phone number: " << database[accno]["phone"];
			cout << "\nEnter the phone number\n";
			readString(new_phone);
			database[accno]["phone"] = new_phone;
			save(database);
			break;
		case 2:
			cout << "\nCurrent email :" << database[accno]["email"];
			cout << "\nEnter the email\n";
			readString(new_email);
			database[accno]["email"] = new_email;
			save(database);
			break;
		case 3:
			cout << "\nEnter current password:\n";
			readString(old_password);
			cout << "\nEnter the new password\n";
			readString(new_password);
			cout << "\nConfirm new password\n";
			readString(confirm_password);
			if (new_password != confirm_password)
			{
				cout << "\nConfirm password did not match" << endl;
				return;
			}
			if (old_password == database[accno]["password"])
				database[accno]["password"] = new_password;
			else
			{
				cout << "\nIncorrect password\n";
				return;
			}
			save(database);
			break;
		case 4:
			return;
		default:
			cout << "Invalid input" << endl;
		}
		cout << "\nDetails updated successfully.\n";
	}
}

/**
 * @parameters: account number and database
 * @description: displays menu after user successfully logs in
 * @options: Display, Withdraw, Deposit, Balance checking, Edit account, Transfer amount, Logout
 */
void userLoggedIn(int accountNumber, unordered_map<int, unordered_map<string, string>> &database)
{
	cout << "\nYour account number is: " << accountNumber;
	while (true)
	{
		cout << "\nSelect an option:\n";
		cout << "1. Diplay account information\n";
		cout << "2. Withdraw amount\n";
		cout << "3. Deposit amount\n";
		cout << "4. Balance checking\n";
		cout << "5. Edit account details\n";
		cout << "6. Transfer amount to another user\n";
		cout << "9. Log out";
		cout << endl;
		int option;
		readInt(option);
		switch (option)
		{
		case 1:
			display(accountNumber, database[accountNumber]);
			break;
		case 2:
			withdraw(accountNumber, database);
			break;
		case 3:
			deposit(accountNumber, database);
			break;
		case 4:
			balance_check(database[accountNumber]);
			break;
		case 5:
			modify(accountNumber, database);
			break;
		case 6:
			transferData(accountNumber, database);
			break;
		case 9:
			cout << "Logging out..\n\n";
			return;
		default:
			cout << "Selected option not found\n";
			break;
		}
	}
}

/**
 * @parameters: database
 * @description: Takes account number and password and logs in the user
 */
void login(unordered_map<int, unordered_map<string, string>> &database)
{
	string input;
	int accountNumber;
	cout << "\nPlease Enter your account number:\n";
	readInt(accountNumber);

	// Search for account number in the database using
	// find function of unordered_map which internally uses hashing
	if (database.find(accountNumber) != database.end())
	{
		cout << "\nEnter password:\n";
		try
		{
			readString(input);
			string userPassword = database[accountNumber]["password"];
			if (input.compare(userPassword) == 0)
			{
				cout << "\nLogin successful!\n";
				userLoggedIn(accountNumber, database);
			}
			else
			{
				cout << "\nPassword didn't match\n";
			}
		}
		catch (exception e)
		{
			cout << "\nDatabase lookup error\n";
		}
	}
}

/**
 * @parameters: database
 * @description: helps user to reset the password after confirming 
 *					answers for secret questions
 */
void forgot(unordered_map<int, unordered_map<string, string>> &database)
{
	int accno;
	string ans1, ans2, ans3, ans4;

	cout << "\nEnter your account number:\n";
	readInt(accno);

	cout << "\nWhat is your birth place?\n";
	readString(ans1);
	ans1 = tolower(ans1);

	cout << "\nWhat is your favourite sport?\n";
	readString(ans2);
	ans2 = tolower(ans2);

	cout << "\nWhat is your favourite color?\n";
	readString(ans3);
	ans3 = tolower(ans3);

	cout << "\nWhat is your mother's name?\n";
	readString(ans4);
	ans4 = tolower(ans4);

	string line, token;
	fstream data;
	data.open(ANSWERS_FILE, ios::out | ios::in);
	if (!data.is_open())
		exit(-1);
	unordered_map<int, unordered_map<string, string>> answers;
	while (getline(data, line))
	{
		stringstream ss(line);
		int j = 0, key, val;
		unordered_map<string, string> columns;
		while (getline(ss, token, ','))
		{
			j++;
			switch (j)
			{
			case 1:
				key = stoi(token, nullptr, 10);
				break;
			case 2:
				columns["ans1"] = token;
				break;
			case 3:
				columns["ans2"] = token;
				break;
			case 4:
				columns["ans3"] = token;
				break;
			case 5:
				columns["ans4"] = token;
				break;
			}
		}
		answers[key] = columns;
	}
	if (answers.find(accno) == answers.end())
	{
		cout << "\nInvalid account number\n";
		return;
	}
	if ((answers[accno]["ans1"] != ans1) || (answers[accno]["ans2"] != ans2) || (answers[accno]["ans3"] != ans3) || (answers[accno]["ans4"] != ans4))
	{
		cout << "\nAnswers did not match\n";
		return;
	}
	for (int i = 0; i < INVALID_ATTEMPTS; i++)
	{
		string new_password, confirm_password;
		cout << "\nEnter new password\n";
		readString(new_password);
		cout << "\nConfirm password\n";
		readString(confirm_password);
		if (new_password == confirm_password)
		{
			database[accno]["password"] = new_password;
			save(database);
			return;
		}
		else
		{
			cout << "\nNew password and confirm password did not match.\n";
		}
	}
}

/**
 * @parameters: database
 * @description: Takes details from user and creates an account. Account number is issued
 * 					by incrementing from max_account
 * @options: Name, Phone, Email, DOB, Aadhaar, Password, Amount
 */
void create(unordered_map<int, unordered_map<string, string>> &database)
{
	unordered_map<string, string> columns;
	string name, password, email, phone, dob, aadhaar;
	string ans1, ans2, ans3, ans4;
	int amount;

	cout << "Enter your name\n";
	getline(cin >> ws, name);
	columns["name"] = name;

	cout << "Enter your phone number\n";
	readString(phone);
	columns["phone"] = phone;

	cout << "Enter your mail id\n";
	readString(email);
	columns["email"] = email;

	cout << "Enter your password\n";
	readString(password);
	columns["password"] = password;

	cout << "Enter your dob\n";
	readString(dob);
	columns["dob"] = dob;

	cout << "Enter your aadhaar number\n";
	readString(aadhaar);
	columns["aadhaar"] = aadhaar;

	cout << "\nWhat is your birth place?\n";
	readString(ans1);
	columns["ans1"] = tolower(ans1);

	cout << "\nWhat is your favourite sport?\n";
	readString(ans2);
	columns["ans2"] = tolower(ans2);

	cout << "\nWhat is your favourite color?\n";
	readString(ans3);
	columns["ans3"] = tolower(ans3);

	cout << "\nWhat is your mother's name?\n";
	readString(ans4);
	columns["ans4"] = tolower(ans4);

	cout << "For creating a new account you should have minimum balance of rupees " << MIN_BAL << "\nPlease enter the amount\n";
	int i = 0;
	while (1)
	{
		readInt(amount);
		if (amount >= MIN_BAL)
		{
			columns["amount"] = to_string(amount);
			int accno = ++max_accno;
			database[accno] = columns;
			save(database);
			saveAnswers(database);
			cout << "\nYOUR DETAILS ARE SUCCESSFULLY SAVED...\n";
			cout << "\nYour account number: " << accno << endl;
			break;
		}
		else
		{
			i++;
			if (i > INVALID_ATTEMPTS)
			{
				cout << "\nAccount creation failed.\n\n";
				return;
			}
			cout << "\nYour account should have minimum balance of " << MIN_BAL << "\nReEnter the amount: ";
		}
	}
}

int main()
{
	string line, token;
	fstream data;
	cout << "Initialising app..\n";
	data.open(DATA_FILE, ios::out | ios::in);
	if (!data.is_open())
		exit(-1);
	unordered_map<int, unordered_map<string, string>> database;
	while (getline(data, line))
	{
		stringstream ss(line);
		int j = 0, key, val;
		unordered_map<string, string> columns;
		while (getline(ss, token, ','))
		{
			j++;
			switch (j)
			{
			case 1:
				key = stoi(token, nullptr, 100);
				if (key > max_accno)
				{
					max_accno = key;
				}
				break;
			case 2:
				columns["name"] = token;
				break;
			case 3:
				columns["phone"] = token;
				break;
			case 4:
				columns["email"] = token;
				break;
			case 5:
				columns["dob"] = token;
				break;
			case 6:
				columns["aadhaar"] = token;
				break;
			case 7:
				columns["password"] = token;
				break;
			case 8:
				columns["amount"] = token;
				break;
			}
		}
		database[key] = columns;
	}
	data.close();
	cout << "App initialised\n";
	string input;
	int option = 0;
	do
	{
		cout << "Select an option\n";
		cout << "1. Login\n";
		cout << "2. Create account\n";
		cout << "3. Forgot password\n";
		cout << "9. Exit\n";
		try
		{
			readInt(option);
			switch (option)
			{
			case 1:
				login(database);
				break;
			case 2:
				create(database);
				break;
			case 3:
				forgot(database);
				break;
			case 9:
				cout << "Exiting app...";
				break;
			default:
				cout << "Please choose one of the available options";
				break;
			}
		}
		catch (exception e)
		{
			cout << "\nInvalid input data!\n";
			continue;
		}
		cout << endl;
	} while (option != 9);
	return 0;
}
