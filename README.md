# servicealertvisualiser

A simple QT application which takes and visualises the data from the Service Alerter repo, by subscribing to the messages from a MQTT server. The QT uses QCustomPlot API for the graphical representation

## Code
- mainwindow.cpp and mainwindow.h 

  - comboBox(Subject to which graph is plotted) dropdown change event -Clears the graph and replots the yLabel based on the selection
  - lineEdit-for topics wildcard entry editing complete – If the value is empty, alerts the user by filling in a warning text in red. That goes back to normal once the user has re-entered the topic to be subscribed to.
  - Connect button click event – Modified this to ensure the topic subscribed for isn’t null and alerts user if so, and resets the Palette colour back to black from alert palette red
  - On message arrival slot – On subscribed json object tree message arrival, the message is parsed using the inbuilt json parsing functionalites, and are stored into its respective global variables and the graph is updated based on this value
  - setGraph() – Plots/Replots the graph and sets the data to be empty. Allows users to zoom in horizontally and drag vertically
  - update() – Implemented logic for automatic graph scaling based on incoming values initially. Once the points go out of view, users would be able to change the view by vertical drag or horizontal zoom in or out.


## Features and Screeshots

### - Manually subscribing to topics
Manual subscription to topics is given through QLineEdit, and hence making it easier for users to change to subscription narrowing down to the sub-topic or opting for a wildcard. The default being the wild card. There are validations been made to ensure this is not left blank.
![](https://i.ibb.co/CsVpGcv/Annotation-2020-05-20-032154.png)
![](https://i.ibb.co/FgHtkVV/Annotation-2020-05-20-2.png)

### - Subject based filtering
Subject based filtering is done through QComboBox or the dropdown, whereby user can select the data to be plotted against time (Temperature, pitch or roll). 
![](https://i.ibb.co/C98bzk4/Annotation-2020-05-20-3.png)

### - Parsing JSON and plotting data in with respect to time
Code implementation for parsing json has been discussed in the previous section. The parsed data is sent as the ‘yData’ along with time data which is set as xData to the customGraph. The UI automatically scales based on the initial values received and user has
ability to zoom horizontally along the time axis or drag vertically based on subject selected.

![](https://i.ibb.co/Qm4PWZL/Annotation-2020-05-20-4.png)
![](https://i.ibb.co/Mk0s8Gv/Annotation-2020-05-20-5.png)


