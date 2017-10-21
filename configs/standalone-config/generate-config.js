const fs = require('fs');
const strip = require('strip-comments');

let template = fs.readFileSync('./config.template', 'utf8');
let code = strip(fs.readFileSync('../dist/app.min.js', 'utf8'));

let finalFile = template.replace('__TIMEBOXED_SCRIPT__', code);

fs.writeFileSync('./generated.js', `module.exports=function(){return "${encodeURIComponent(finalFile)}";}`);
