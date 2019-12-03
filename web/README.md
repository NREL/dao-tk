# UI Dashboard

The UI dashboard is based on [Django](https://www.djangoproject.com/), which is a high-level Python web framework. In order to have interactive plots that are driven by Python code executed real-time, a charting server is needed. The open-source visualization library [Bokeh](https://bokeh.org/) is used for this purpose. Bokeh can be run a number of different ways, but running as a charting server is necessary to have Python-driven interative plots.

The Bokeh server can be run alongside Django same as before, including while debugging in Visual Studio Code. However, in order to do so on a local machine, with or without an additional web server (e.g., Apache), you must add another loopback address. Normally, on the production server, the Bokeh charting server will utilize the default loopback address of 127.0.0.1 and the web server will utilize the public facing IP address. On a local machine, however, the IP address cannot be repurposed in this way so another loopback address must be added, for either the the web server or Django's manage.py module. In order to get another loopback address, a loopback adapter must be installed.

For production testing, the web server [Waitress](https://docs.pylonsproject.org/projects/waitress/en/stable/) is used along with the [Nginx](https://www.nginx.com/) web server. The Nginx server is configured as a public-facing reverse proxy that passes the outside web requests to the Waitress server, but predominantly it is needed to serve the static files.

**Steps to get the Bokeh server working alongside Django**
1. Obtain a second loopback address for Django or the web server. The original loopback address, 127.0.0.1, is now used by the Bokeh charting server.
  1. In a Command Prompt (cmd.exe) type: hdwwiz.exe
  2. Choose manual option, then Network adapters
  3. Choose Microsoft->Microsoft KM-TEST Loopback Adapter
  4. After it is installed, find the name of the adapter by typing "ipconfig /all" in a Command Prompt and looking for the Loopback Adapter entry. For example, the name is "Ethernet 5" if the block entry name is "Ethernet adapter Ethernet 5:"
  5. Configure the IP address, subnet mask and gateway. Choose a non-routable IP address like 10.10.10.10 (preferred). Configure this by entering this command at the command prompt: "netsh int ip set address "Ethernet 5" static 10.10.10.10 255.255.255.0 10.10.10.254 1" . This is for the case of the name being 'Ethernet 5' and the chosen IP being 10.10.10.10
  6. Verify the change by running "ipconfig /all" again
2. Add these system environment variables:
  1.  BOKEH_SECRET_KEY : 
  2.  BOKEH_SIGN_SESSIONS : False  (or True)   -> is this needed?
3. Start the Bokeh charting server in a dedicated prompt (I use an Anaconda Prompt as 'bokeh' is in the Path and thus recognized in any directory)
  1. Navigate to the directory with the Bokeh apps: "\dao-tk\web\site\ui\BokehApps\"
  2. Run:  bokeh serve sliders.py --allow-websocket-origin 10.10.10.10 --address 127.0.0.1
  3. You can add addition python files after sliders.py, separated by a space
  4. Note: this would eventually be done using a new, custom service
4. In a new prompt, start the Django server as usual but now use the new loopback address: 'python manage.py runserver 10.10.10.10:80' or debug in Visual Studio code using the 'Python: Django 10.10.10.10:80' configuration
5. Sometimes the Bokeh charting server websocket closes and won't reopen automatically when navigating away from a page with a Bokeh plot. Not sure why. When this happens, the Bokeh charting server needs to be restarted.

**Using a web server instead of manage.py**
1. 	Install a web server, in this case the Python "Waitress" package
2.  Add a reverse-proxy, in this case Nginx. This functions to forward requests from port 80 to port 8000 but more importantly serves the static files. For Windows:
  1.  Download the latest mainline version of Nginx: http://nginx.org/en/docs/windows.html
  2.  Extract and place files in C:\Program Files\nginx-X.X.X\
  3.  Configure nginx by replacing \conf\nginx.conf with the preconfigured file in this repo.
  4.  Anytime this config file is edited, restart Nginx, which can be done using the included restart_nginx.bat batch file.
3. Start the Nginx reverse proxy by running nginx.exe or the included batch file
4. Start the Waitress webserver using a dedicated  prompt (e.g., Anaconda Prompt for the same reason)
  1. Navigate to the main directory of the project: \dao-tk\web\site\
  2. Run:   waitress-serve --listen="10.10.10.10:8000" dtkweb.wsgi:application
  3. Note: this would eventually be done using a new, custom service
5. Open a web browser to:  http://10.10.10.10/ui    (which is port 80)
6. Navigate to the Outlook page to see an example Bokeh interactive chart.
  1. The chart is fully created in a python file
  2. Any changes to the input sliders call callback functions in this file
  3. Arbitrary Python code is executed and new plot data is created and the chart is automatically updated

**Explanation**
  * Navigating to http://10.10.10.10/ui   (implicit port 80) is intercepted by the Nginx reverse proxy and routed to http://10.10.10.10:8000/ui
  * http://10.10.10.10:8000 is the address of the Waitress web server . However, web servers shouldn't/can't be used to serve static content, like the images and CSS, and this is the purpose of the upstream Nginx reverse proxy.
  * You can navigate directly to http://10.10.10.10:8000 to see what the website looks like when bypassing the Nginx proxy, and thus without the static file content. Also, the Bokeh charts will not work as the source port of 8000 is not being allowed by the Bokeh server.