<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<meta http-equiv="X-UA-Compatible" content="IE=edge">
	<meta name="viewport" content="width=device-width, initial-scale=1.0">
	<title>Result form</title>
</head>
<body>
<?php
if (isset($_POST['test']))
	echo "RESULTAT POST : {$_POST['test']}<br>";
if (isset($_GET['test']))
	echo "RESULTAT GET : {$_GET['test']}<br>";

$uploaddir = 'upload/';

if (isset($_FILES['userfile']))
{
	$uploadfile = $uploaddir . basename($_FILES['userfile']['name']);

	echo '<pre>';
	if (move_uploaded_file($_FILES['userfile']['tmp_name'], $uploadfile)) {
		echo "Le fichier est valide, et a été téléchargé avec succès. Voici plus d'informations :\n";
	} else {
		echo "Attaque potentielle par téléchargement de fichiers. Voici plus d'informations :\n";
	}

	echo 'Voici quelques informations de débogage :';
	print_r($_FILES);

	echo '</pre>';
}
echo "<a href=\"/\">return index</a>";
?>
</body>
</html>