//used styling imports
import './App.css';
import 'bootstrap/dist/css/bootstrap.min.css';
//used react imports
import {useState, useEffect} from 'react';
import { initializeApp } from 'firebase/app';
import { getDatabase, ref, onValue} from "firebase/database";
//used bootstrap styles
import Container from 'react-bootstrap/Container';
import Row from 'react-bootstrap/Row';
import Col from 'react-bootstrap/Col'
//image imports
import humidImg from './img/humidimg.png';
import tempImg from './img/tempimg.png';
import hrImg from './img/hrimg.png';

//firebase setup 
const firebaseConfig = {

};

//connect to firebase realtime database 
initializeApp(firebaseConfig);
//reference for the database 
const db = getDatabase();


function App() {
    //get the database section where data is stored from the user (esps) that are uploading data
    const dbRef = ref(db, 'UsersData/eacN42h5UVVCQamxQoTXUjGV9P53');
    //hold values for output 
    const [temp, setTemp] = useState("");
    const [humid, setHumid] = useState("");
    const [hr, setHr] = useState("");
    

    // useEffect will update the webpage on change 
    useEffect(() => {
      //onValue will update the outputs when the database changes 
      onValue(dbRef, (snapshot) => {
        //data from the database in object form
        const data = snapshot.val();
        //set output values 
        setTemp("Temperature: " + data.DHT.temperature);
        setHumid("Humidity: " + data.DHT.humidity);
        setHr("Heart Rate: " + data.HeartRate.heartRate);
      });
    });
  
    //return webpage with images and outputs
    return (
     <Container>
      <Row>
        <Col md="auto"><img src ={tempImg} alt="Temperature"/></Col>
        <Col md={4}><h1> {temp}</h1></Col>
      </Row>
      <Row>
        <Col md="auto"><img src ={humidImg} alt="Humidty"/> </Col>
        <Col md={4} ><h1>{humid} </h1> </Col>
      </Row>
      <Row>
        <Col md="auto"><img src ={hrImg} alt="Heart Rate"/></Col>
        <Col md={4}><h1>{hr}</h1></Col>
      </Row>
    </Container>
    );
  }

export default App;
