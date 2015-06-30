# openversa_C_v1
Initial C Binding for REST api.openversa.com interface. The FULL REST API documentation is located https://api.openversa.com under the api section. To sign up and recieve your API Keys email ed@miversainc.com and the request will be completed. You will be able to create groups and message any person or device.

Within the files be sure to replace the API KEY, SECRET and USER EMAIL fields with your openversa development credentials

API Reference

The Openversa REST API allows you to create groups, add members, send and process messages across all message channels. You can also upload files for storage, schedule messages and send files across all channel types.

Since the API is based on REST principles, it’s very easy to write and test applications. You can use your browser to access URLs, and you can use pretty much any HTTPS client in any programming language to interact with the API.

JSON will be returned in all responses from the API, including errors (though if you’re using API bindings, we will convert the response to the appropriate language-specific object).

Account Owners

An Account owner is the same name as your username is automatically created for you when you sign up. By default, you are assigned as the administrator of this organization


Container Data Model & Object Hierarchy Concept

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

Account Owner: This is the root and owner of the groups. Once you are registered with Openversa as a developer, you will get assigned an application ID and secret key.

Groups: Groups are containers that contain members, storage, rules, analytics and devices.

Members: Groups contain members. Members can be people, devices, accounts etc…

Files: Files are store internal by groups. Files can be send to any member and will be optimized for that platform and sent with Openversa short URL

Datastore: Application data can be stored per container using this datastore

Business rules: All event management, routing, processing are all handled by business rules

Analytics: All activity monitoring is done on a per group and all group basis

Properties Properties of a container are set here such as description or location
