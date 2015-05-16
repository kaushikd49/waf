###Web Application Firewall

A web application firewall (WAF) is a system running in-front of a web server that is responsible of inspecting all traffic coming from the Internet and relaying it to the actual web server. If a WAF sees traffic that contains an attack, it should just drop the request and do not forward it to the application. WAFs have the ability to protect (to a certain extent) vulnerable web applications by ensuring that malicious traffic never reaches the vulnerable code.

In this project, we designed and implemented a Web Application Firewall that "sits" in-front of a web server. We implemented the WAF as an Apache Module. The WAF recognizes malicious requests using two methods: signatures and anomaly detection.

The details of the project are in the [report].

#####Detection Methods

*   Signatures

    In this mode, your WAF should support a database of signatures that are characteristic of maliciousness. You should design a way to encode signatures, so that you can specify where exactly is the string that you want to match. For example,signature that should be supported are:
      HEADER:User-Agent,CONTAINS:"<script>"	/*Detect basic XSS ATTACK through User-Agent Header*/
    HEADER:User-Agent,CONTAINS:"bot"	/*Deny all requests from hosts that identify themselves as bots*/
    REQUEST_METHOD:GET,PARAMETER:*,CONTAINS:"union all select" /*Search all parameters of GET Requests for possible SQL Injection*/
    REQUEST_METHOD:POST,PARAMETER:foo,CONTAINS:"../../../../" /*Search the parameter "foo" of POST Requests for possible Directory Traversal vulnerabilities*/   

*   Anomaly Detection

    While signatures are a great way to stop known attacks, they can't stop attacks that do not have corresponding signatures. In anomaly detection, we can "learn" how legitimate traffic looks like and then if some future traffic is too different from what we've learned, we do not allow it to proceed. Here, we split the system in a training phase and an online phase. In the training phase, the WAF does not stop anything. It just collects information about all traffic passing through it. The user is then trying to use the website as much as possible, click on all possible links, and perform all possible actions. When the user is satisfied that she has exercised the web application as much as possible, it asks the WAF to create a normality profile. The WAF will look through all requests and try to make generalized rules. For this project, I want you to learn automatically the following things:
    Maximum number of parameters seen for all requests across all pages
    Maximum number of parameters seen for every specific page
    Average length of values for every specific parameter of every specific page
    E.g. the average length of the values of the parameter foo on example.com/index.php is 12 characters
    Character set of any specific parameter
    E.g. the parameter name on example.com/signup.phpwas always comprised out of letters and numbers, but never out of special characters
    The detection should happen as follows:
    For profiles involving maximum number of parameters, drop requests that exceed that maximum number
    For profiles involving averages, assume a normal distribution, compute the standard deviation, and drop all values that are not part of the mean+-3*sd
    For profiles involving character sets, drop requests with parameters containing character from sets not seen during training

####Overall

Together with the WAF we delivered a web application where the WAF is trained upon.

[report]:https://github.com/kaushikd49/waf/blob/master/NetSec%20Project%20Report.pdf
