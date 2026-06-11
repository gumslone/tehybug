<?php
// Shared CORS headers: the pages are served from tehybug.com but fetched
// by the dashboard that the device itself serves on its local IP.
header("Access-Control-Allow-Origin: *");
header("Access-Control-Allow-Methods: POST, GET, OPTIONS");
header("Access-Control-Allow-Headers: Origin, X-Requested-With, Content-Type, Accept");
