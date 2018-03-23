const wi = require('../');

const db = new wi.Database(process.argv[2], wi.MSIDBOPEN_READONLY);
const view = db.openView(process.argv[3]);

view.execute();

let record = null;

while ((record = view.fetch())) {
    let result = "";

    for (let i = 1; i < record.getFieldCount(); i++) {
        result += "," + record.getString(i);
    }

    console.log(result.substr(1));
}
