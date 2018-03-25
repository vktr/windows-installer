const wi = require('../');
const db = new wi.Database(process.argv[2], wi.MSIDBOPEN_READONLY);
const preview = db.enableUIPreview();

preview.previewDialog(process.argv[3]);
console.log('Press Ctrl+C to close dialog and exit.');

setInterval(() => {}, 1000);
