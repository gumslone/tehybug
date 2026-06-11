<?php
// Bundles all stylesheets into one response. ob_gzhandler negotiates
// gzip with the client on its own.
header("Content-type: text/css");
ob_start('ob_gzhandler');

$files = array(
    'files/bootstrap.min.css',
    'files/gumboard.css',
);

foreach ($files as $file) {
    readfile(dirname(__FILE__) . '/' . $file);
    echo "\n";
}
