<?php
session_start();

$_SESSION["test"] = "ok";
$_COOKIE["test"] = "ok";
setcookie("test", "ok");
?>

<a href="/coo_sess/sessions.php">View value</a>