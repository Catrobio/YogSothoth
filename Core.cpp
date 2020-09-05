#include <algorithm>
#include <fstream>
#include <ctime>
#include <string>
#include <../../postgresql/libpq-fe.h>
#include "lvcResult.cpp"
#include "Functions.cpp"

using namespace std;

class Core
{
private:
	PGconn *cnn = NULL;
	PGresult *result = NULL;
	char *host;
	char *port;
	char *dataBase;
	char *user;
	char *passwd;
	char *Qry;
	Function f;
	string err, strHost, strPort, strDataBase, strSchema, strUser, strPassword;
	bool Connect()
	{
		string line;
		int con = 0;
		ifstream fileconf("/Conf.lvc");
		fileconf.open("Conf.lvc");
		if (fileconf.is_open())
		{
			while (!fileconf.eof())
			{
				getline(fileconf, line);
				if (con == 0)
				{
					strHost = f.Descrypt(line);
				}
				if (con == 1)
				{
					strPort = f.Descrypt(line);
				}
				if (con == 2)
				{
					strDataBase = f.Descrypt(line);
				}
				if (con == 3)
				{
					strSchema = f.Descrypt(line);
				}
				if (con == 4)
				{
					strUser = f.Descrypt(line);
				}
				if (con == 5)
				{
					strPassword = f.Descrypt(line);
				}
				con++;
			}
			host = (char *)strHost.c_str();
			port = (char *)strPort.c_str();
			dataBase = (char *)strDataBase.c_str();
			user = (char *)strUser.c_str();
			passwd = (char *)strPassword.c_str();
			cnn = PQsetdbLogin(host, port, NULL, NULL, dataBase, user, passwd);
			if (PQstatus(cnn) != CONNECTION_BAD)
				return true;
			else
			{
				err =  f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^","");
				return false;
			}
		}
		else
		{
			perror("Conf.lvc");
			return false;
		}
	}

	bool Disconnect()
	{
		PQclear(result);
		PQfinish(cnn);
		return true;
	}

	bool ValidQuery(string sql)
	{
		int tf = 0;
		if (f.Isreplace(sql, "ALTER ", ""))
			tf += 1;
		if (f.Isreplace(sql, "DROP ", ""))
			tf += 1;
		if (f.Isreplace(sql, "TRUNCATE ", ""))
			tf += 1;
		if (f.Isreplace(sql, "LOCKET ", ""))
			tf += 1;
		if (f.Isreplace(sql, "TABLE ", ""))
			tf += 1;
		if (f.Isreplace(sql, "SHOW ", ""))
			tf += 1;
		if (f.Isreplace(sql, "DATABASE ", ""))
			tf += 1;
		if (f.Isreplace(sql, "DECLARE ", ""))
			tf += 1;
		if (tf == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool ValitQueryToken(string sql)
	{
		int tf = 0;
		if (ValidQuery(sql))
		{
			sql = f.UpperString(sql);
			if (f.Isreplace(sql, "INSERT ", ""))
				tf += 1;
			if (f.Isreplace(sql, "UPDATE ", ""))
				tf += 1;
			if (f.Isreplace(sql, "SELECT ", ""))
				tf += 1;
			if (f.Isreplace(sql, "DELETE ", ""))
				tf += 1;
		}
		if (tf == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool ValidQuerySelect(string sql)
	{
		int tf = 0;
		sql = f.UpperString(sql);
		if (ValidQuery(sql))
		{
			if (f.Isreplace(sql, "INSERT ", ""))
				tf += 1;
			if (f.Isreplace(sql, "UPDATE ", ""))
				tf += 1;
			if (f.Isreplace(sql, "DELETE ", ""))
				tf += 1;
		}
		else
		{
			return false;
		}
		if (tf == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool ValidQueryDelete(string sql)
	{
		int tf = 0;
		if (ValidQuery(sql))
		{
			sql = f.UpperString(sql);
			if (f.Isreplace(sql, "INSERT ", ""))
				tf += 1;
			if (f.Isreplace(sql, "UPDATE ", ""))
				tf += 1;
		}
		if (tf == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool ValidQueryUpdate(string sql)
	{
		int tf = 0;
		if (ValidQuery(sql))
		{
			sql = f.UpperString(sql);
			if (f.Isreplace(sql, "INSERT ", ""))
				tf += 1;
			if (f.Isreplace(sql, "DELETE ", ""))
				tf += 1;
		}
		if (tf == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	bool ValidQueryInsert(string sql)
	{
		int tf = 0;
		if (ValidQuery(sql))
		{
			sql = f.UpperString(sql);
			if (f.Isreplace(sql, "UPDATE ", ""))
				tf += 1;
			if (f.Isreplace(sql, "DELETE ", ""))
				tf += 1;
		}
		if (tf == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	lvcResult SetToken(string UserName, string Pass, int cas)
	{
		string tkenresult = "", qry;
		lvcResult Res;
		Res.CodeStatus = 0;
		Res.Data = "";
		if (ValitQueryToken(UserName) && ValitQueryToken(Pass))
		{
			if (UserName != "" && Pass != "")
			{
				char *vari = "abcdefghijklmnopqrstuvwxyz1234567890QWERTYUIOPASDFGHJKLZXCVBNM";
				int p;
				srand(time(0));
				if (Connect())
				{
					try
					{
						string TryPass = f.EncryptToken(f.Trim(Pass));
						if (cas == 0)
							qry = "SELECT idUser FROM \"" + strSchema + "\".\"lvc_users\" WHERE UserName = '" + f.Trim(UserName) + "' AND Pass= '" + TryPass + "' AND  iduserstatus = 1";
						else
							qry = "SELECT idUser FROM \"" + strSchema + "\".\"lvc_users\" WHERE UserName = '" + f.Trim(UserName) + "' AND Pass= '" + TryPass + "' AND iduserstatus IS NULL";
						Qry = (char *)qry.c_str();
						result = PQexec(cnn, Qry);
						if (PQresultStatus(result) != PGRES_TUPLES_OK)
						{
							err =  f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^","");
							Res.CodeStatus = -5;
							Res.Msg = "Query erro in to user: "+err ;
							
						}
						else
						{
							int col = 1;
							for (int i = 0; i < 32; i++)
							{
								p = (1 + rand() % 61);
								tkenresult += vari[p];
							}
							int rows = PQntuples(result);
							int fields = PQnfields(result);
							string user;
							if (fields != 0 && rows != 0)
							{
								for (int i = 0; i < fields; i++)
									for (int j = 0; j < rows; j++)
										user = PQgetvalue(result, j, i);
								if (user != "" && user.size() != 0)
								{
									qry = "";
									qry = "INSERT INTO  \"" + strSchema + "\".\"lvc_userauthentication\" (UserToken,DateConnection,DateExpiration,InfoConnection,IsActive,User_IdUser) values ('" + f.EncryptToken(tkenresult) + "',NOW(),(NOW() + interval '365 day'),'192.168.1.1','1','" + user + "')";
									Qry = (char *)qry.c_str();
									result = PQexec(cnn, Qry);
									if (PQresultStatus(result) == PGRES_COMMAND_OK)
									{
										Res.CodeStatus = 1;
										Res.Msg = "";
										Res.Data = tkenresult;
									}
									else
									{
										err =  f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^","");
										Res.CodeStatus = -8;
										Res.Msg = "Erro in Authenticaction: "+err ;
										
									}
								}
								else
								{
									err =  f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^","");
									Res.CodeStatus = -5;
									Res.Msg = "Can not create user: "+err ;
									
								}
							}
							else
							{
								Res.CodeStatus = -6;
								Res.Msg = "Invalid user ";								
							}
						}
					}
					catch (std::exception const &e)
					{
						Res.CodeStatus = -1;
						Res.Msg = "Null references error";
					}
				}
				else
				{
					Res.CodeStatus = -7;
					Res.Msg = "Connection to Token error: " ;
					
				}
			}
			else
			{
				Res.CodeStatus = -3;
				Res.Msg = "Please inser user and password to get token.";
			}
		}
		else
		{
			Res.CodeStatus = -2;
			Res.Msg = "Invalid query token. User or Pass containt invalid phrase.";
		}
		Disconnect();
		return Res;
	}

	lvcResult SetUser(string qry, string user, string pass)
	{
		lvcResult Res;
		Res.CodeStatus = 0;
		Res.Data = "";
		qry = f.replace(qry, "\\", "");
		if (ValitQueryToken(qry) && ValitQueryToken(user) && ValitQueryToken(pass))
		{
			if (Connect())
			{
				string Tran, BTran = "BEGIN", Ctran = "COMMIT;", Rtran = "ROLLBACK;", SavePoint = "Set_User_Point";
				try
				{
					Tran = "SELECT UserName FROM  \"" + strSchema + "\".\"lvc_users\" where UserName = '" + user + "'";
					Qry = (char *)Tran.c_str();
					result = PQexec(cnn, Qry);
					if (PQresultStatus(result) == PGRES_TUPLES_OK || PQresultStatus(result) == PGRES_COMMAND_OK)
					{
						int rows = PQntuples(result);
						int fields = PQnfields(result);
						if (rows == 0)
						{
							pass = f.Trim(pass);
							Tran = "INSERT INTO \"" + strSchema + "\".\"lvc_users\" (DateBirthday,IdTypeAuthentication,Identy,LastName,Names,Pass,UserName,emailuser) VALUES " + qry;
							Qry = (char *)Tran.c_str();
							result = PQexec(cnn, Qry);
							if (PQresultStatus(result) == PGRES_COMMAND_OK)
							{								
								string Npass = f.EncryptToken(pass);
								Tran = "UPDATE  \"" + strSchema + "\".\"lvc_users\" set Pass ='" + Npass + "' where UserName = '" + user + "' and Pass ='" + pass + "'";
								Qry = (char *)Tran.c_str();
								result = PQexec(cnn, Qry);
								if (PQresultStatus(result) == PGRES_COMMAND_OK)
								{
									Disconnect();
									return SetToken(user, pass, 1);
								}
								else
								{
									err =  f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^","");
									Res.CodeStatus = -7;
									Res.Msg = "Error in udate info user: "+err ;
									
								}
							}
							else
							{
								err =  f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^","");
								Res.CodeStatus = -6;
								Res.Msg = "Error in insert user: "+err ;
								
							}
						}
						else
						{
							Res.CodeStatus = -5;
							Res.Msg = "User name exist ";
						}
					}
					else
					{
						err =  f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^","");
						Res.CodeStatus = -4;
						Res.Msg = "Error geting user info: "+err ;
						
					}
				}
				catch (exception &e)
				{
					Res.CodeStatus = -3;
					Res.Msg = "Null references error: ";
				}
			}
			else
			{
				Res.CodeStatus = -2;
				Res.Msg = "Connection set user error: " ;
				
			}
		}
		else
		{
			Res.CodeStatus = -1;
			Res.Msg = "invalid Query";
		}
		Disconnect();
		return Res;
	}

	lvcResult ChangeUser(string user, string tk)
	{
		lvcResult Res;
		Res.CodeStatus = 0;
		if (ValitQueryToken(user) && ValitQueryToken(tk))
		{
			if (Connect())
			{
				try
				{
					string BTran = "BEGIN", Ctran = "COMMIT;", Rtran = "ROLLBACK;", SavePoint = "Set_User_Point";
					string validToken = "SELECT IdUserAuthentication FROM \"" + strSchema + "\".\"lvc_userauthentication\" a INNER JOIN \"" + strSchema + "\".\"lvc_users\" b ON  a.user_iduser  = b.iduser  where a.UserToken='" + f.EncryptToken(f.Trim(tk)) + "' AND b.username = '" + f.Trim(user) + "' AND NOW() <= DateExpiration AND IsActive = 1 AND b.iduserstatus IS NULL";
					Qry = (char *)validToken.c_str();
					result = PQexec(cnn, Qry);
					if (PQresultStatus(result) == PGRES_TUPLES_OK)
					{
						int rows = PQntuples(result);
						int fields = PQnfields(result);
						if (fields != 0 && rows != 0)
						{
							string qry = "UPDATE  \"" + strSchema + "\".\"lvc_users\" set idUserType = 3, iduserstatus = 1, ispublic = 1, datecreate = NOW() where UserName= '" + f.Trim(user) + "'";
							Qry = (char *)qry.c_str();
							result = PQexec(cnn, Qry);
							if (PQresultStatus(result) == PGRES_COMMAND_OK)
								Res.CodeStatus = 1;
							else
							{
								err =  f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^","");
								Res.CodeStatus = -6;
								Res.Msg = "Erorr Seting Token: "+err;
							}
						}
						else
						{
							Res.CodeStatus = -5;
							Res.Msg = "Invalid Token ";							
						}
					}
					else
					{
						err =  f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^","");
						Res.CodeStatus = -4;
						Res.Msg = "Error in user authentication: "+err;
						
					}					
				}
				catch (exception &e)
				{
					Res.CodeStatus = -3;
					Res.Msg = "Null references error";
				}
			}
			else
			{
				Res.CodeStatus = -2;
				Res.Msg = "Erro to connect to database: " ;
				
			}
			Disconnect();
		}
		else
		{
			Res.CodeStatus = -1;
			Res.Msg = "Invalid query token. User or Pass containt invalid phrase.";
		}
		return Res;
	}

	lvcResult Getqry(string qry, string user, string tk)
	{
		lvcResult Res;
		Res.CodeStatus = 0;
		Res.Msg = "";
		Res.Data = "";
		if (ValitQueryToken(qry) && ValitQueryToken(user) && ValitQueryToken(tk))
		{
			if (ValidQuery(qry))
			{
				qry = f.replace(qry, "\\", "");
				if (Connect())
				{
					try
					{
						string Tran, BTran = "BEGIN", Ctran = "COMMIT;", Rtran = "ROLLBACK;", SavePoint = "Set_User_Point";
						string validToken = "Select IdUserAuthentication FROM \"" + strSchema + "\".\"lvc_userauthentication\" where UserToken='" + f.EncryptToken(f.Trim(tk)) + "' AND NOW() <= DateExpiration AND IsActive = 1";
						Tran = validToken;
						Qry = (char *)Tran.c_str();
						result = PQexec(cnn, Qry);
						if (PQresultStatus(result) == PGRES_TUPLES_OK)
						{

							int rows = PQntuples(result);
							int fields = PQnfields(result);
							Res.CodeStatus = 1;
							if (fields != 0 && rows != 0)
							{
								Qry = (char *)qry.c_str();
								result = PQexec(cnn, Qry);
								if (PQresultStatus(result) == PGRES_COMMAND_OK)
								{
									rows = PQntuples(result);
									fields = PQnfields(result);
									Res.CodeStatus = 1;
									if (fields != 0 && rows != 0)
									{
										for (int j = 0; j < rows; j++)
										{
											Res.Data = Res.Data + "{";
											for (int i = 0; i < fields; i++)
											{
												Res.Data = Res.Data + "\"" + PQfname(result, i) + "\"" + ":";
												Res.Data = Res.Data + "\"" + PQgetvalue(result, j, i) + "\"" + ",";
											}
											Res.Data = f.RTrim(Res.Data, ",");
											Res.Data = Res.Data + "},";
										}
										Res.Data = f.RTrim(Res.Data, ",");
									}
									else
									{
										Res.Msg = "{}";
									}
								}
								else
								{
									err =  f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^","");
									Res.Msg = "wrong command: "+err ;
									Res.CodeStatus = -700;
								}
							}
							else
							{
								Res.CodeStatus = -2;
								Res.Msg = "Invalid Token";
								Disconnect();
							}
						}
						else
						{
							err =  f.replaceAll(f.replaceAll(f.replaceAll(PQerrorMessage(cnn),"\n"," "),"\""," "),"^","");
							Res.Msg = "Erro to get token: "+err ;
							Res.CodeStatus = -701;
						}
						Disconnect();
					}
					catch (std::exception const &e)
					{
						Res.CodeStatus = -1;
						Res.Msg = " Null references error";
						Disconnect();
					}
				}
				else
				{
					Res.CodeStatus = -101;
					Res.Msg = "Connection to Get error: " ;
				}
				Disconnect();
			}
			else
			{
				Res.CodeStatus = -2;
				Res.Msg = "Invalid Query!";
			}
		}
		else
		{
			Res.CodeStatus = -3;
			Res.Msg = "Invalid Query!";
		}
		return Res;
	}

	bool GetUserExist(string UserName)
	{
		bool ed = false;
		if (ValitQueryToken(UserName))
		{
			string token = "", qry;
			if (UserName != "")
			{
				wstring qryc;
				int p;
				if (Connect())
				{
					try
					{
						string TryUser = UserName;
						qry = "SELECT idUser FROM \"" + strSchema + "\".\"lvc_users\" WHERE UserName = '" + UserName + "'";
						Qry = (char *)qry.c_str();
						result = PQexec(cnn, Qry);
						if (PQresultStatus(result) == PGRES_TUPLES_OK)
						{
							int rows = PQntuples(result);
							if (rows != 0)
							{
								ed = true;
							}
							else
							{
								ed = false;
							}
						}
						else
						{
							ed = false;
						}
						Disconnect();
					}
					catch (std::exception const &e)
					{
						Disconnect();
						return false;
					}
				}
				else
				{
					ed = false;
				}
			}
			else
			{
				ed = false;
			}
		}
		return ed;
	}

public:
	lvcResult PlayToken(string UserName, string Pass)
	{
		lvcResult Res;
		try
		{
			return SetToken(UserName, Pass, 0);
		}
		catch (std::exception const &e)
		{
			Res.CodeStatus = -1;
			Res.Msg = " Error getting Token";
		}

		return Res;
	}

	lvcResult CreateUser(string qry, string user, string pass)
	{
		return SetUser(qry, user, pass);
	}

	lvcResult UpdateUser(string user, string tk)
	{
		lvcResult Res;
		try
		{
			Res = ChangeUser(user, tk);
		}
		catch (std::exception const &e)
		{
			Res.CodeStatus = -11;
			Res.Msg = " Error al ejecutar la operacion";
		}

		return Res;
	}

	lvcResult GetTokenInfo(string qry, string user, string tk)
	{
		lvcResult Res;
		try
		{
			Res = Getqry(qry, user, tk);
		}
		catch (std::exception const &e)
		{
			Res.CodeStatus = -704;
			Res.Msg = " Error al ejecutar la operacion";
		}

		return Res;
	}

	lvcResult UserExist(string user)
	{
		lvcResult Res;
		string useralt, altr, rslt = "";
		useralt = f.Trim(user);
		int i = 1;
		try
		{
			Res.CodeStatus = -1;
			if (!GetUserExist(useralt))
			{
				Res.CodeStatus = 1;
				Res.Data = "\"User\": \"" + user + "\"";
			}
			else
			{
				while (i < 4)
				{
					std::string s = std::to_string(i);
					useralt = f.Trim(user) + "_" + s;
					if (!GetUserExist(useralt))
					{
						rslt += useralt + ";";
						i = i + 1;
					}
				}
				if (i == 4)
				{
					Res.Data = "\"User\":\"" + rslt + "\"";
					Res.CodeStatus = 1;
				}
			}
		}
		catch (std::exception const &e)
		{
			Res.CodeStatus = -704;
			Res.Msg = " Error al ejecutar la operacion";
		}

		return Res;
	}
};