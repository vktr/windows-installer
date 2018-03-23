# Node.js bindings for the Windows Installer API

This package lets you open, query and modify MSI files from Node.js. It
provides some abstraction on top of the native Windows Installer API but
otherwise just passes through any arguments.

The API is currently synchronous and will throw errors at the Windows Installer
API's behalf.


## Usage

### Install the package

```sh
$ npm install --save windows-installer
```

### Query the database

```js
const wi = require('windows-installer');

const db = new wi.Database('C:/Installer.msi', wi.MSIDBOPEN_READONLY);
const view = db.openView("SELECT * FROM _Tables");

view.execute();

let record = null;

while ((record = view.fetch())) {
    console.log(record.getString(1));
}
```


## API coverage

### Database functions

Database functions requires a `Database` (named `db` in the examples
below). This can be obtained by running `new Database(<Path>, <PersistMode>)`.

| Win32 Function                 | Node.js function                     |
| ------------------------------ | ------------------------------------ |
| `MsiDatabaseCommit`            | `db.commit()`                        |
| `MsiDatabaseGetPrimaryKeys `   | `db.getPrimaryKeys(<TableName>)`     |
| `MsiDatabaseIsTablePersistent` | `db.isTablePersistent(<TableName>)`  |
| `MsiDatabaseOpenView`          | `db.openView(<Query>)`               |
| `MsiOpenDatabase`              | `new Database(<Path>, <PeristMode>)` |


### View functions

View functions requires a `View` (named `view` in the examples below). This
can be obtained by running `openView` on a `Database` instance.

| Win32 Function                 | Node.js function                     |
| ------------------------------ | ------------------------------------ |
| `MsiViewGetColumnInfo`         | `view.getColumnInfo(<InfoType>)`     |
| `MsiViewClose`                 | `view.close()`                       |
| `MsiViewExecute`               | `view.execute()`                     |
| `MsiViewFetch`                 | `view.fetch()`                       |
| `MsiViewGetError`              | `view.getError()`                    |
| `MsiViewModify`                | `view.modify(<Mode>, <Record>)`      |
