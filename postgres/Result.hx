package postgres;

class Result {

	public static final EMPTY_QUERY = 0;
	public static final COMMAND_OK = 1;
	public static final TUPLES_OK = 2;
	public static final COPY_OUT = 3;
	public static final COPY_IN = 4;
	public static final BAD_RESPONSE = 5;
	public static final NONFATAL_ERROR = 6;
	public static final FATAL_ERROR = 7;
	public static final COPY_BOTH = 8;
	public static final SINGLE_TUPLE = 9;
	public static final PIPELINE_SYNC = 10;
	public static final PIPELINE_ABORTED = 11;
	public static final TUPLES_CHUNK = 12;

	var h : Dynamic;

	public function new( h ) {
		this.h = h;
	}

	public function status() : Int {

		return Postgres.postgres_result_status( h );
	}

	public function error() : Null<String> {

		var b = Postgres.postgres_result_error( h );
		var len = Postgres.postgres_result_error_length( h );
		return b == null ? null : b.toBytes( len ).toString();
	}

	public function isSuccess() : Bool {

		return switch ( status() ) {
			case COMMAND_OK, TUPLES_OK, SINGLE_TUPLE, TUPLES_CHUNK: true;
			default: false;
		};
	}

	public function numRows() : Int {

		return Postgres.postgres_num_rows( h );
	}

	public function numFields() : Int {

		return Postgres.postgres_num_fields( h );
	}

	public function fieldName( field : Int ) : Null<String> {

		var b = Postgres.postgres_field_name( h, field );
		var len = Postgres.postgres_field_name_length( h, field );
		return b == null ? null : b.toBytes( len ).toString();
	}

	public function isNull( row : Int, field : Int ) : Bool {

		return Postgres.postgres_is_null( h, row, field );
	}

	public function getString( row : Int, field : Int ) : Null<String> {

		var b = Postgres.postgres_get_value( h, row, field );
		var len = Postgres.postgres_get_length( h, row, field );
		return b == null ? null : b.toBytes( len ).toString();
	}

	public function getBytes( row : Int, field : Int ) : Null<haxe.io.Bytes> {

		var b = Postgres.postgres_get_value( h, row, field );
		var len = Postgres.postgres_get_length( h, row, field );
		return b == null ? null : b.toBytes( len );
	}

	public function getInt( row : Int, field : Int ) : Null<Int> {

		var value = getString( row, field );
		return value == null ? null : Std.parseInt( value );
	}

	public function getFloat( row : Int, field : Int ) : Null<Float> {

		var value = getString( row, field );
		if ( value == null )
			return null;

		var parsed = Std.parseFloat( value );
		return Math.isNaN( parsed ) ? null : parsed;
	}

	public function affectedRows() : Int {

		return Postgres.postgres_cmd_tuples( h );
	}

	public function clear() : Void {

		Postgres.postgres_result_clear( h );
	}
}
