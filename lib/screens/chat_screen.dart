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
        title: Text('⚡️Data'),
        backgroundColor: Colors.lightBlueAccent,
      ),
      body: SafeArea(
        child: ListView(
          children: <Widget>[
            Padding(
              padding: const EdgeInsets.fromLTRB(0, 20, 0, 10),
              child: Center(
                  child: Text(
                    'Real-Time Sensor Data',
                    style: TextStyle(
                        fontSize: 25,
                        fontWeight: FontWeight.bold
                    ),
                  ),
                ),
            ),
            DataTable(
              columns: [
                DataColumn(
                  label: Text(
                    'Sensor Name',
                    style: TextStyle(
                      fontSize: 18,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                ),
                DataColumn(
                  label: Text(
                    'Sensor Value',
                    style: TextStyle(fontSize: 18,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                ),
              ],
              rows: [
                //Vibration Sensor - X
                DataRow(
                  cells: [
                    DataCell(Text('Vibration Sensor - X'),),
                    DataCell(
                      StreamBuilder(
                        stream: _database.ref("vibrationX").onValue,
                        builder: (context, snap) {
                          if (!snap.hasData) {
                            return Text(snap.data.snapshot.value.toString());
                          }
                          else {return Text("loading...");}
                        },
                      ),
                    ),
                  ],
                ),
                //Vibration Sensor - Y
                DataRow(
                  cells: [
                    DataCell(Text('Vibration Sensor - Y'),),
                    DataCell(
                      StreamBuilder(
                        stream: _database.ref("VibrationY").onValue,
                        builder: (context, snap) {
                          if (!snap.hasData) {
                            return Text(snap.data.snapshot.value.toString());
                          }
                          else {return Text("loading...");}
                        },
                      ),
                    ),
                  ],
                ),
                //Current Sensor
                DataRow(
                  cells: [
                    DataCell(Text('Current Sensor'),),
                    DataCell(
                      StreamBuilder(
                        stream: _database.ref("Current").onValue,
                        builder: (context, snap) {
                          if (!snap.hasData) {
                            return Text(snap.data.snapshot.value.toString());
                          }
                          else {return Text("loading...");}
                        },
                      ),
                    ),
                  ],
                ),
                //Temperature Sensor
                DataRow(
                  cells: [
                    DataCell(Text('Temperature Sensor'),),
                    DataCell(
                      StreamBuilder(
                        stream: _database.ref("tempComp").onValue,
                        builder: (context, snap) {
                          if (!snap.hasData) {
                            return Text(snap.data.snapshot.value.toString());
                          }
                          else {return Text("loading...");}
                        },
                      ),
                    ),
                  ],
                ),
                //RPM
                DataRow(
                  cells: [
                    DataCell(Text('RPM'),),
                    DataCell(
                      StreamBuilder(
                        stream: _database.ref("rpm").onValue,
                        builder: (context, snap) {
                          if (!snap.hasData) {
                            return Text(snap.data.snapshot.value.toString());
                          }
                          else {return Text("loading...");}
                        },
                      ),
                    ),
                  ],
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }
}

