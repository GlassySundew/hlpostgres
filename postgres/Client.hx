package postgres;

class Client {

	var h : Dynamic;

	public function new( conninfo : String ) {

		h = Postgres.postgres_connect( @:privateAccess conninfo.toUtf8() );
		if ( h == null )
			throw "Postgres connect failed";
	}

	public function query( sql : String ) : Result {

		var r = Postgres.postgres_query( h, @:privateAccess sql.toUtf8() );
		if ( r == null )
			throw lastErrorOr( "Postgres query failed" );

		var result = new Result( r );
		if ( !result.isSuccess() ) {
			var message = result.error();
			result.clear();
			throw message != null ? message : "Postgres query failed";
		}

		return result;
	}

	public function execute( sql : String ) : Int {

		var result = query( sql );
		try {
			var affected = result.affectedRows();
			result.clear();
			return affected;
		} catch ( e ) {
			result.clear();
			throw e;
		}
	}

	public function lastError() : Null<String> {

		var b = Postgres.postgres_last_error( h );
		var len = Postgres.postgres_last_error_length( h );
		return b == null ? null : b.toBytes( len ).toString();
	}

	public function isOk() : Bool {

		return Postgres.postgres_is_ok( h );
	}

	public function close() : Void {

		Postgres.postgres_close( h );
	}

	function lastErrorOr( fallback : String ) : String {

		var err = lastError();
		return err != null && err != "" ? err : fallback;
	}
}
