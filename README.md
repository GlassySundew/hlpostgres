# hashlink/c postgres bindings

## Compilation
* Place repo at your `hashlink/libs` directory

### Platform
##### Windows
* install PostgreSQL client development files so `libpq-fe.h` and `libpq.lib` are available
* if needed, set `PostgreSQL_ROOT` to your PostgreSQL installation path
* run `build.bat` at repo root

##### Linux
* install libpq dev files, for example `sudo apt install libpq-dev`
* run `make` inside repo root

## Usage
The client accepts a normal libpq connection string:

```haxe
var db = new postgres.Client( "host=127.0.0.1 port=5432 dbname=app user=app password=secret" );
var result = db.query( "select id, name from users order by id limit 10" );

for ( row in 0...result.numRows() ) {
	trace( result.getString( row, 0 ) + ": " + result.getString( row, 1 ) );
}

result.clear();
db.close();
```

Convenience helpers:
* `execute( sql ) : Int` for statements where you only care about affected rows
* `query( sql ) : Result` for statements returning a result set or command metadata
* `lastError() : Null<String>`
* `isOk() : Bool`

