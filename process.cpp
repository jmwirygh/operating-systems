# include < stdlib .h >
# include < sys / types .h >
# include < sys / wait .h >
# include < cstring >
# include < iostream >
# include < fstream >
# include < string >
# include < unistd .h >
# include < ctime >
using namespace std ;
# define MAX_COUNT 20
# define READ_FD 0
# define WRITE_FD 1
int pipefd [2];
int status ;
char sendBuf [50] , receiveBuf [50];
string line ;
int count = 1;
string devdata [100][3];


int devcount = 0;
void parentProcess ( void )
{
	write ( pipefd [ WRITE_FD ] , line . c_str () , line . length () +1) ;
	cout << " Parent sent file line " << count ++ << endl ;
	wait (& status ) ;
	int n = read ( pipefd [ READ_FD ] , sendBuf , 50) ;
	char mess [3][30];
	int c = 0;
	for (int i = 0; i < 3; i ++)
	{
			int k = 0;
			while ( sendBuf [ c ] != ’\0 ’ && c < 50)
			{
				mess [ i ][ k ] = sendBuf [ c ];
				c ++;
				k ++;
			}
	    c ++;
	    mess [ i ][ k ] = ’\0 ’;
	}
	string device = mess [0];
	string value = mess [1];
	string time = mess [2];
	cout << " Parent received update for " << device << ": " << value
	<< " at " << time << endl ;
	int exist = -1;
	
	for (int j = 0; j < devcount ; j ++)
	{
		if ( devdata [ j ][0] == device )
		exist = j ;
	}
	
	if ( exist >= 0)
	{
		devdata [ exist ][1] = value ;
		devdata [ exist ][2] = time ;
	}
	else
	{
		devdata [ devcount ][0] = device ;
		devdata [ devcount ][1] = value ;
		devdata [ devcount ][2] = time ;
		devcount ++;
	}
}
void childProcess ( void )
{
		read ( pipefd [ READ_FD ] , receiveBuf , 50) ;
		cout << " Child received message line : " << receiveBuf << endl ;
		// D e v i c e
		char * rest = strstr ( receiveBuf , ",") ;
		int pos = rest - receiveBuf ;
		char device [20];
		strncpy ( device , receiveBuf , pos ) ;
		device [ pos ] = ’\0 ’;
		//New s u b s t r i n g
		char sub [50];
		strncpy ( sub , rest +1 , strlen ( rest ) ) ;
		// V alue
		char * rest1 = strstr ( sub , ",") ;
		int pos1 = rest1 - sub ;
		char value [20];
		strncpy ( value , sub , pos1 ) ;
		value [ pos1 ] = ’\0 ’;
		//Time
		char timech [11];
		strncpy ( timech , rest1 +1 , strlen ( rest1 ) ) ;
		time_t time = ( time_t ) atoll ( timech ) ;
		struct tm * timeinfo = localtime (& time ) ;
		char timestr [30];
		strftime ( timestr , 30 , "%H:%M:%S %d/%m/%Y", timeinfo ) ;
		sleep (1) ;
		close ( pipefd [ READ_FD ]) ;
		write ( pipefd [ WRITE_FD ] , device , strlen ( device ) +1) ;
		cout << " Child sent device : " << device << endl ;
		write ( pipefd [ WRITE_FD ] , value , strlen ( value ) +1) ;
		cout << " Child sent value : " << value << endl ;
		write ( pipefd [ WRITE_FD ] , timestr , strlen ( timestr ) +1) ;
		cout << " Child sent time : " << timestr << endl ;
		close ( pipefd [ WRITE_FD ]) ;
		exit (0) ;
}


int main ()
{
		pid_t pid ;
		pipe ( pipefd ) ;
		ifstream inputFile ;
		inputFile . open (" device_data .txt") ;
		while ( inputFile >> line )
		{
		     pid = fork () ;
		    if ( pid == 0)
		      {
		         childProcess () ;	
		      }
		    else
			{
				 parentProcess () ;
			}
		}
		
		wait (& status ) ;
		cout << " ============ " << endl ;
		cout << " Device List :" << endl ;
		cout << " ============ " << endl ;
		for (int k = 0; k < devcount ; k ++)
		{
			cout << devdata [ k ][0] << ":\t" << devdata [ k ][1] << "\t@ " <<
			devdata [ k ][2] << endl ;
		}
		exit (0) ;
}