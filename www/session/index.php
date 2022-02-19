<?php
session_start();

$_SESSION["test"] = rand();
echo $_SESSION["test"];
if (!isset($_COOKIE["test"]))
	setcookie("test", "ok");
?>

<a href="/session/sessions.php">view session info</a>