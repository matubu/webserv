<?php
session_start();

$_SESSION["test"] = rand();
echo $_SESSION["test"];
//$_COOKIE["test"] = "ok";
//setcookie("test", "ok");
?>

<a href="/coo_sess/sessions.php">View value</a>