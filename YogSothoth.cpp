#include <stdlib.h>
#include "Core.cpp"

using namespace std;

constexpr unsigned int str2int(const char* str, int h = 0)
{
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
} 

int main(int narg, char* argv[]) {
	Core Core;
	lvcResult qry;
	string TormResult;
	string valid, equ;
	string x;
	char* y;
	Function f;
	try
	{		
		printf("Content-type:application/json\n\n");
		for (int i = 1; i < narg; i++) {
			valid += argv[i];
			valid += " ";
		}
		valid = f.replaceAll(valid, "¡-", "(");
		valid = f.replaceAll(valid, "-¡", ")");		
		valid = f.replaceAll(valid, "!/", "\"");	
		vector<string> e = f.split(valid, "¿");
		x = e[0];
		y = (char *)x.c_str();
		switch (str2int(y))
		{
		case -437126392: //GetToken
			qry = Core.PlayToken(e[1], e[2]);
			TormResult = "{\"Data\":[\"" + qry.Data + "\"], \"CodeStatus\":\"" + to_string(qry.CodeStatus) + "\", \"Msg\" : \"" + qry.Msg + "\"}";
			cout << TormResult;
			break;
		case 1713924228: //SetLogin
			qry = Core.CreateUser(e[1], f.Trim(e[2]), f.Trim(e[3]));
			TormResult = "{\"Data\":[\"" + qry.Data + "\"], \"CodeStatus\":\"" + to_string(qry.CodeStatus) + "\", \"Msg\" : \"" + qry.Msg + "\"}";			
			cout << TormResult;
			break;		
		case -1379718589: //UpLogin						
			qry = Core.UpdateUser(e[1], e[2]);
			TormResult = "{\"Data\":[\"" + qry.Data + "\"], \"CodeStatus\":\"" + to_string(qry.CodeStatus) + "\", \"Msg\" : \"" + qry.Msg + "\"}";
			cout << TormResult;
			break;
		case 1692413396: //TkInfo
			equ = f.replaceAll(e[1], "!s", "=");
			qry = Core.GetTokenInfo(equ, e[2], e[3]);
			TormResult = "{\"Data\":[" + qry.Data + "], \"CodeStatus\":\"" + to_string(qry.CodeStatus) + "\", \"Msg\" : \"" + qry.Msg + "\"}";
			cout << TormResult;
			break;
		case 1732998936: //UsrExt			
			qry = Core.UserExist(e[1]);
			TormResult = "{\"Data\":[{" + qry.Data + "}], \"CodeStatus\":\"" + to_string(qry.CodeStatus) + "\", \"Msg\" : \"" + qry.Msg + "\"}";
			cout << TormResult;
			break;
		default:
			return 0;
			break;
		}
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
		cout << "Error de ejecucion";
	}
	return 0;
}