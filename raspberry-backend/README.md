## High Energy Ventilator   

### Backend and frontend for homemade ventilators

- Mail: adam.abed.abud@cern.ch
- Last update: April 2, 2020


**Backend:** python, flask, sqlite 
**Frontend:** Javascript, HTML 


# Usage
Before starting the web application you will need the following packages to be installed on the RP:

```sh
sudo pip3 install flask
sudo apt-get install sqlite3
```



```sh
python3 arduino_recorder.py
```
This will start the server and it will simulate data coming from two different sensors



Start the web application. 

```sh
python3 app.py
```

Start firefox with the following address:

```sh
firefox 127.0.0.1:5000/new
```


License
----

For the benefit of everyone.



