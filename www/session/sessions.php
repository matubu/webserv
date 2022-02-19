<?php
session_start();

if (isset($_SESSION["test"]))
	echo $_SESSION["test"] . "<br>";

if (isset($_COOKIE["test"]))
	echo "Ok Cookie<br>";

