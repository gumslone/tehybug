<?php
// Bundles all scripts into one response. jQuery must come before
// gumboard.js (it runs jQuery code at parse time). The "\n;\n" between
// files keeps a trailing "//# sourceMappingURL=..." comment in one
// minified file from commenting out the start of the next.
header("Content-type: text/javascript");
ob_start('ob_gzhandler');

$files = array(
    'files/jquery-3.6.0.min.js',
    'files/bootstrap.min.js',
    'files/feather.min.js',
    'files/gumboard.js',
);

foreach ($files as $file) {
    readfile(dirname(__FILE__) . '/' . $file);
    echo "\n;\n";
}
