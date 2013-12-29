/* timeKiller.cpp

	Written ( quickly ) by John Bell for CS 385, Spring 2006.

	Illustrates use of argc, argv, envp, and then chews up clock cycles.

*/

#include <iostream>
#include <cmath>

using namespace std;

int main( int argc, char ** argv, char ** envp ) {
	// Equivalent to int argc, char * argv[ ], char * envp[ ] ) {

	int i;

	cout << "\nargc = " << argc << endl;
	
	cout << "\nargv = \n";
	for( i = 0; i < argc; i++ )
		cout << "\targv[ " << i << " ] = " << argv[ i ] << endl;

	cout << "\nenvp = \n";
	for( char ** e = envp; *e; e++ )
		cout << "\tenvp[ " << i++ << " ] = \"" << *e << "\"\n";

	if( argc <= 1 || atoi( argv[ 1 ] ) < 2 ) {
		cout << "\nUsage: " << argv[ 0 ] << " [ int int int ]\n\n";
		return 0;
	}

	// NOTE:  The following code is deliberately inefficient!
	// Its purpose in life is to chew up clock cycles!

	double sum = 0.0;
	int old = 0;
	int last = 1;
	int current = 2;
	double *data = new double[ atoi( argv[ 1 ] ) ];
	srand( time( 0 ));
	data[ 0 ] = 1.223 * rand( ) / RAND_MAX * atoi( argv[ 1 ] );
	data[ 1 ] = 1.235 * rand( ) / RAND_MAX * atoi( argv[ 1 ] );
	for( i = 0; i < 2; i++ ) {
		while( fabs( data[ i ] ) < 1.234E-10 )
			data[ i ] = 2.45* rand( ) / RAND_MAX * atoi( argv[ 1 ] );

		while( fabs( data[ i ] ) < 10.04829 )
			data[ i ] *= 10.2357;

		while( fabs( data[ i ] ) > 5.04829E6 )
			data[ i ] /= 10.2357;
	}
	//cout << data[ 0 ] << "     " << data[ 1 ] << endl;

	int limit1 = 1, limit2 = 1;
	if( argc >= 3 )
		limit1 = atoi( argv[ 2 ] );
	if( argc >= 4 )
		limit2 = atoi( argv[ 3 ] );

	long count = 0;
	int frequency = 1000;
	cout << "\nPreparing to crunch numbers.  What printing frequency do you desire? > ";
	cin >> frequency;
	cout << endl;

	for( int k = 0; k < limit2; k++ )
	for( int j = 0; j < limit1; j++ )
	for( i = 0; i < atoi( argv[ 1 ] ); i++ ) {

		if( !( count++ % frequency ) ) {
			cout << '.';
			cout.flush( );
		}
		if( !( count % ( 60 * frequency ) ) )
			cout << endl;

		data[ current ] = exp( fabs( pow( data[ last ], 2.38 ) / pow(  data[ old ], 3.215 ) ) ) ;
		//cout << data[ current ] << endl;
		
		while( fabs( data[ current ] ) < 1.234E-10 )
			data[ current ] = 3.21*rand( ) / RAND_MAX * atoi( argv[ 1 ] );

		while( fabs( data[ current ] ) < 10.04829 )
			data[ current ] *= 10.2357;

		while( fabs( data[ current ] ) > 5.04829E6 )
			data[ current ] /= 10.2357;
		
		sum += data[ old ] * data[ last ] * data[ current ];
		//cout << sum << endl;

		while( fabs( sum ) < 1.234E-10 )
			sum = 1.3243 * rand( ) / RAND_MAX * atoi( argv[ 1 ] );

		while( fabs( sum ) < 10.04829 )
			sum *= 10.2357;

		while( fabs( sum ) > 5.04829E6 )
			sum /= 10.2357;

		old = last;
		last = current;
		current = ( current + 1 ) % atoi( argv[ 1 ] );
	}

	cout << "\nsum = " << sum << endl;

	return 0;

} // main
