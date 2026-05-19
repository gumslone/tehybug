<?php
/**
 * JavaScript Asset Bundler
 *
 * This script concatenates multiple JavaScript files into a single response,
 * handles GZIP compression, and sets appropriate caching headers.
 */

// --- Configuration ---

// Define the list of JavaScript files to include.
// The order is important as files are concatenated sequentially.
$jsFiles = [
    './files/bootstrap.min.js',
    './files/jquery-3.6.0.min.js',
    './files/jquery.weekline.min.js',
    './files/gumboard.js',
];

// Set cache lifetime in seconds. 10 minutes = 600  seconds.
$cacheDuration = 60 * 10; // 10 minutes

// --- Asset Delivery ---

// Set content type and caching headers.
header("Content-type: text/javascript; charset=utf-8");
header("Cache-Control: max-age=" . $cacheDuration . ", public");
header("Expires: " . gmdate("D, d M Y H:i:s", time() + $cacheDuration) . " GMT");

// Enable GZIP compression if supported by the client.
if (isset($_SERVER['HTTP_ACCEPT_ENCODING']) && strpos($_SERVER['HTTP_ACCEPT_ENCODING'], 'gzip') !== false) {
    header("Content-Encoding: gzip");
    header("Vary: Accept-Encoding");
    ob_start('ob_gzhandler');
} else {
    ob_start();
}

// Concatenate and output the contents of the JavaScript files.
foreach ($jsFiles as $file) {
    if (file_exists($file)) {
        readfile($file);
        // Add a newline to prevent issues with files that don't end with one.
        echo "\n";
    }
}

ob_end_flush();