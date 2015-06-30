# openversa_C_v1
Initial C Binding for REST api.openversa.com interface. The FULL REST API documentation is located https://api.openversa.com under the api section. To sign up and recieve your API Keys email ed@miversainc.com and the request will be completed. You will be able to create groups and message any person or device.

Within the files be sure to replace the API KEY, SECRET and USER EMAIL fields with your openversa development credentials

<h3>API Reference</h3>

The Openversa REST API allows you to create groups, add members, send and process messages across all message channels. You can also upload files for storage, schedule messages and send files across all channel types.

Since the API is based on REST principles, it’s very easy to write and test applications. You can use your browser to access URLs, and you can use pretty much any HTTPS client in any programming language to interact with the API.

JSON will be returned in all responses from the API, including errors (though if you’re using API bindings, we will convert the response to the appropriate language-specific object).

<h3>Account Owners</h3>

An Account owner is the same name as your username is automatically created for you when you sign up. By default, you are assigned as the administrator of this organization


<h3>Container Data Model & Object Hierarchy Concept</h3>
<style>
#content .body p > code {
  display: block;
  padding: 15px;
  white-space: pre;
}
#content .body code {
  border: 1px solid #e2e7ea;
  background: #31373A;
  border-radius: 2px;
  -moz-border-radius: 5px;
  -webkit-border-radius: 5px;
  color: #C1EF65;
  padding: 0 1px;
}
code {
  font-family: 'Courier', monospace, sans-serif;
}
</style>
<code>
Account Owner (main root, app id and secret key)
    {
       Groups (containers) 
         {
            Members (1 or many, people, devices, accounts)
            Messages (outgoing, incoming)
            Files (storage) 
            Datastore
            Business rules
            Analytics
            Properties
         }
       Groups (containers) 
         {
            Members (1 or many, people, devices, accounts)
            Messages (outgoing, incoming)
            Files (storage) 
            Datastore
            Business rules
            Analytics
            Properties
         }
     }
</code>
<h4>Account Owner:</h4> This is the root and owner of the groups. Once you are registered with Openversa as a developer, you will get assigned an application ID and secret key.

<h4>Groups:</h4> Groups are containers that contain members, storage, rules, analytics and devices.

<h4>Members:</h4> Groups contain members. Members can be people, devices, accounts etc…

<h4>Files:</h4> Files are store internal by groups. Files can be send to any member and will be optimized for that platform and sent with Openversa short URL

<h4>Datastore:</h4> Application data can be stored per container using this datastore

<h4>Business rules:</h4> All event management, routing, processing are all handled by business rules

<h4>Analytics:</h4> All activity monitoring is done on a per group and all group basis

<h4>Properties:</h4> Properties of a container are set here such as description or location
