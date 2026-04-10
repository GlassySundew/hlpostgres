package postgres;

private typedef PostgresHandle = hl.Abstract<"hl_postgres">;
private typedef PostgresResultHandle = hl.Abstract<"hl_postgres_result">;

@:hlNative( "postgres" )
class Postgres {

	public static function postgres_connect( conninfo : hl.Bytes ) : PostgresHandle {
		return null;
	}

	public static function postgres_close( handle : PostgresHandle ) : Void {}

	public static function postgres_last_error( handle : PostgresHandle ) : hl.Bytes {
		return null;
	}

	public static function postgres_last_error_length( handle : PostgresHandle ) : Int {
		return 0;
	}

	public static function postgres_is_ok( handle : PostgresHandle ) : Bool {
		return false;
	}

	public static function postgres_query( handle : PostgresHandle, sql : hl.Bytes ) : PostgresResultHandle {
		return null;
	}

	public static function postgres_result_status( result : PostgresResultHandle ) : Int {
		return 0;
	}

	public static function postgres_result_error( result : PostgresResultHandle ) : hl.Bytes {
		return null;
	}

	public static function postgres_result_error_length( result : PostgresResultHandle ) : Int {
		return 0;
	}

	public static function postgres_num_rows( result : PostgresResultHandle ) : Int {
		return 0;
	}

	public static function postgres_num_fields( result : PostgresResultHandle ) : Int {
		return 0;
	}

	public static function postgres_field_name( result : PostgresResultHandle, field : Int ) : hl.Bytes {
		return null;
	}

	public static function postgres_field_name_length( result : PostgresResultHandle, field : Int ) : Int {
		return 0;
	}

	public static function postgres_is_null( result : PostgresResultHandle, row : Int, field : Int ) : Bool {
		return false;
	}

	public static function postgres_get_value( result : PostgresResultHandle, row : Int, field : Int ) : hl.Bytes {
		return null;
	}

	public static function postgres_get_length( result : PostgresResultHandle, row : Int, field : Int ) : Int {
		return 0;
	}

	public static function postgres_cmd_tuples( result : PostgresResultHandle ) : Int {
		return 0;
	}

	public static function postgres_result_clear( result : PostgresResultHandle ) : Void {}
}
