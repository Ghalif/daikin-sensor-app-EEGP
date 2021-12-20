import 'package:firebase_database/firebase_database.dart';
import 'package:flutter/material.dart';
import 'package:flash_chat/constants.dart';
import 'package:firebase_auth/firebase_auth.dart';
import 'package:firebase_database/firebase_database.dart';

class ChatScreen extends StatefulWidget {
  static String id = 'chat_screen';
  @override
  _ChatScreenState createState() => _ChatScreenState();
}

class _ChatScreenState extends State<ChatScreen> {
  final _auth = FirebaseAuth.instance;
  final _database = FirebaseDatabase.instance;
  User loggedInUser;
  var listen = "no";

  @override
  void initState() {
    super.initState();
    getCurrentUser();
  }

  void getCurrentUser() async {
    try {
      final user = await _auth.currentUser;
      if (user != null) {
        loggedInUser = user;
        print(loggedInUser);
      }
    }
    catch (e) {
      print(e);
    }
  }

  readData() async {
    var data;
    //final ref = await _database.ref("message");
    //Stream<DatabaseEvent> stream = ref.onValue;
    await _database.ref("message").onValue.listen((DatabaseEvent event) {
      print('Snapshot: ${event.snapshot.value}');
      data = event.snapshot.value;
    });
    /*stream.listen((DatabaseEvent event) {
      print('Snapshot: ${event.snapshot.value}');
      data = event.snapshot.value;
      return data;
    });*/
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        leading: null,
        actions: <Widget>[
          IconButton(
              icon: Icon(Icons.logout),
              onPressed: () async {
                await _auth.signOut();
                Navigator.pop(context);
              }),
        ],
        title: Text('⚡️Chat'),
        backgroundColor: Colors.lightBlueAccent,
      ),
      body: SafeArea(
        child: Column(
          mainAxisAlignment: MainAxisAlignment.spaceBetween,
          crossAxisAlignment: CrossAxisAlignment.stretch,
          children: <Widget>[
            StreamBuilder(
                stream: _database.ref("message").onValue,
                builder: (context, snap) {
                  if (!snap.hasData) return Text("loading...");
                  return Text(snap.data.snapshot.value.toString());
                },
            ),
            StreamBuilder(
              stream: _database.ref("number").onValue,
              builder: (context, snap) {
                if (!snap.hasData) return Text("loading...");
                return Text(snap.data.snapshot.value.toString());
              },
            ),
            Container(
              decoration: kMessageContainerDecoration,
              child: Row(
                crossAxisAlignment: CrossAxisAlignment.center,
                children: <Widget>[
                  Expanded(
                    child: Text (
                        "$listen"
                    ),
                  ),
                  FlatButton(
                    onPressed: () {
                      readData();
                    },
                    child: Text(
                      'Send',
                      style: kSendButtonTextStyle,
                    ),
                  ),
                ],
              ),
            ),
          ],
        ),
      ),
    );
  }
}
