# Fichier de configuration file.conf
## Choix du port et de l’host de chaque "serveur"
- host pour moi ces server_name
- port = listen dans nginx (Dans nginx peut en avoir plusieurs)
## setup du server_name
- server_name www.trucbidule.fr trucbidule.fr;
- nom de domaine du serveur arrière, possibilité d'en spécifier plusieurs, pour nginx mais le sujet dit **DU** server_name du coup je me demande si ces pas juste 1.
## Le premier serveur pour un host :port sera le serveur par défaut pour cet host :port (ce qui signifie qu’il répondra à toutes les requêtes qui n’appartiennent pas à un autre serveur)
- Pour moi cela ve dire que le premier fichier de config doit etre le par default si pas de config trouver
## Limitation de la taille du body des clients
```
Syntaxe: client_max_body_size size;
Défaut:	 client_max_body_size 1m ;
```
- Définit la taille maximale autorisée du corps de la requête client. Si la taille d'une demande dépasse la valeur configurée, l'erreur 413 (Request Entity Too Large) est renvoyée au client. Veuillez noter que les navigateurs ne peuvent pas afficher correctement cette erreur. La définition size sur 0 désactive la vérification de la taille du corps de la requête client.
- taille max des données envoyées par un client
## setup des pages d’erreur par défaut
```
error_page 400 401 402 403 404 405 406 407 408 409 410 411 412 413 414 415 416 417 500 501 502 503 504 505 506 507 /error.html;
```
- redirection des erreurs suivantes vers le chemin indiqué
## setup des routes avec une ou plusieurs des règles suivantes (les routes n’utiliseront pas de regexp) :
- définit une liste de méthodes HTTP acceptées pour la route
	- Methode HTTP = GET, POST, ... 
- définit une redirection HTTP.
```
	location /override {
			return 301 https://$host/somewhere;
	}
	location / {
			return 301 https://$host$request_uri;
	}
	return [code redirection 30x] http://website.fr/index.html
```
## définit un répertoire ou un fichier à partir duquel le fichier doit être recherché (par exemple si l’url /kapouet est rooté sur /tmp/www, l’url /kapouet/pouic/toto/pouet est /tmp/www/pouic/toto/pouet )
```
root /data/www;
location / {
	root /data/www;
}
location /images/ {
	root /data;
}
```
## activer ou désactiver la liste des répertoires
```
Syntax:	autoindex on | off;
Default: autoindex off;
```
#
## Un fichier par défaut comme réponse si la requête est un répertoire
#
## exécute CGI en fonction de certaines extensions de fichier (par exemple .php)
```
location ~* \.php$ {
    fastcgi_index   index.php;
    fastcgi_pass    127.0.0.1:9000;
    include         fastcgi_params;
    fastcgi_param   SCRIPT_NAME $fastcgi_script_name;
}
```
#
## Parce que vous n’allez pas appeler le CGI utilisez directement le chemin complet comme PATH_INFO
- https://fr.wikipedia.org/wiki/Variables_d%27environnement_CGI
#
## Souvenez vous simplement que pour les requêtes fragmentées, votre serveur doit la dé-fragmenter et le CGI attendra EOF comme fin du body.
#
## Mêmes choses pour la sortie du CGI. si aucun content_length n’est renvoyé par le CGI, EOF signifiera la fin des données renvoyées.
#
## Votre programme doit appeler le cgi avec le fichier demandé comme premier argument
#
## le cgi doit être exécuté dans le bon répertoire pour l’accès au fichier de chemin relatif
#
## votre serveur devrait fonctionner avec un seul CGI (php-cgi, python...)
