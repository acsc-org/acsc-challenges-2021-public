<?php
require dirname(__FILE__).DIRECTORY_SEPARATOR."lib/config.php";
require dirname(__FILE__).DIRECTORY_SEPARATOR."lib/User.class.php";
require dirname(__FILE__).DIRECTORY_SEPARATOR."lib/Admin.class.php";
require dirname(__FILE__).DIRECTORY_SEPARATOR."lib/functions.php";

$id = $_REQUEST['id'];
$pw = $_REQUEST['pw'];
$acc = [$id, $pw];
main($acc);
