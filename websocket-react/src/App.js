import React, { useEffect, useState, useRef } from "react";
import * as speechCommands from "@tensorflow-models/speech-commands";
import * as tf from "@tensorflow/tfjs";
import { LoadingButton } from "@mui/lab";
import { Typography, Card, CardContent, Avatar } from "@mui/material";
import MicIcon from "@mui/icons-material/Mic";
import CloudIcon from '@mui/icons-material/Cloud';
import CloudOffIcon from '@mui/icons-material/CloudOff';
import "./App.css";

// "browser": { "fs": false, "node-fetch": false, "string_decoder": false, "crypto": false },

function App() {
  const [number, setNumber] = useState(".");
  const numberRef = useRef(number);
  numberRef.current = number;
  const [recording, setRecording] = useState(false);
  const [loading, setLoading] = useState(false);
  const [connected] = useState(false);
  const [recognizer, setRecognizer] = useState();

  useEffect(() => {
    const loadModel = async () => {
      await tf.ready();
      let recognizer = speechCommands.create("BROWSER_FFT");
      setRecognizer(recognizer);
    };
    loadModel();
  }, [setNumber]);

  useEffect(() => {
    const NUMBERARRAY = [
      "zero",
      "one",
      "two",
      "three",
      "four",
      "five",
      "six",
      "seven",
      "eight",
      "nine",
      "_background_noise_",
      "_unknown_",
    ];
    const makeRequest = async (number) => {
      setRecording(false);
      const options = {
        method: "POST",
        mode: "cors",
        credentials: "same-origin",
        headers: {
          "Content-Type": "application/json",
        },
        redirect: "follow",
        referrerPolicy: "no-referrer",
        body: JSON.stringify({ number }),
      };
      console.log("Sending Post Request with options:", options);
      const response = await fetch("http://localhost:5000/api", options);
      console.log("Received Response: ", response);
      const data = await response.json();
      console.log(data);
      setLoading(false);
    };

    const predictWord = async (recognizer) => {
      await recognizer.ensureModelLoaded();
      const words = recognizer.wordLabels();
      recognizer.listen(
        ({ scores }) => {
          scores = Array.from(scores).map((s, i) => ({
            score: s,
            word: words[i],
          }));
          // Find the most probable word.
          scores.sort((s1, s2) => s2.score - s1.score);
          const filteredScores = scores.filter((score) =>
            NUMBERARRAY.includes(score.word)
          );
          setRecording(false);
          setNumber(filteredScores[0].word);
        },
        { probabilityThreshold: 0.75 }
      );
    };
    if (recording) {
      predictWord(recognizer);
      setTimeout(() => {
        recognizer.stopListening();
        makeRequest(numberRef.current);
      }, 5000);
    }
  }, [recognizer, number, recording]);

  return (
    <div className="App">
      <Card variant="outlined" sx={{boxShadow:3, minWidth: '200px'}}>
        <CardContent className="card-content">
          <Typography variant="h5">Project Enable Smart Oven</Typography>
          <Typography variant="h3">{number}</Typography>
          <div className='status-record-box'>
            <Avatar sx={{bgcolor: `${connected ? '#4caf50' : '#e91e63'}`}}>
              {connected ? <CloudIcon /> : <CloudOffIcon />}
            </Avatar>
            <LoadingButton
              variant="contained"
              onClick={() => {
                setRecording(true);
                setLoading(true);
              }}
              loading={loading}
              startIcon={<MicIcon />}
            >
              Record
            </LoadingButton>
          </div>
        </CardContent>
      </Card>
    </div>
  );
}

export default App;
