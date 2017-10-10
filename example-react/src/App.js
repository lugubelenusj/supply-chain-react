import React, { Component } from 'react';
import logo from './logo.svg';
import './App.css';

class App extends Component {
    constructor() {
        super();

        this.state = {
            urls: [],
            price: '',
            description: '',
            data: 'email address'
        };

        this.updateState = this.updateState.bind(this);
    }

   componentDidMount() {
       var self = this;
       fetch('https://api.github.com/gists', {
           method: 'get'
       }).then(function(response) {
           // return [
           //     {
           //         "orderNo": "3333332",
           //         "description": "Tshirt",
           //         "price": "20"
           //     },  /* json[0] */
           //     {
           //         "orderNo": "32",
           //         "description": "pants",
           //         "price": "30"
           //     }    /* json[1] */
           // ]
           return response.json();
          //fawe fawefw
           // fawjelfkjwelkfj
           // fawelfkjwelkfj

           /**
            * flaewjflkwjfkljawe
            * fawjflkwjefklwje
            * flkjawelkfj
            */
       }).then(function(json) {
           console.log(json);


           self.setState({
              price: json[0]['created_at'],
               description: json[1]
           });

       }).catch(function(err) {
           // Error :(
       });

   }

    updateState(e) {
        this.setState({data: ''});
    }

  render() {
    return (
      <div className="App">
        <header className="App-header">
          {/*<img src={logo} className="App-logo" alt="logo" />*/}
          <h1 className="App-title">Supply Chain</h1>
            {/*<div>I'm inside react</div>*/}
        </header>
        <p className="App-intro">
          To get started, sign up or log in please and save to reload.
        </p>


          <div>this is what we have</div>

          <div>
              <input type = "text" value = {this.state.data}
                     onChange = {this.updateState} />
              <h4>{this.state.data}</h4>
          </div>


          {/*<div className="container">*/}
              {/*<div className="row">*/}
                  {/*<div className="col">*/}
                      {/*<div>Price: {this.state.price}</div>*/}
                  {/*</div>*/}
                  {/*<div className="col">*/}
                      {/*<div>Order Number: {this.state.description.orderNo}</div>*/}
                  {/*</div>*/}
              {/*</div>*/}
              {/*<div className="row">*/}
                  {/*<div className="col">*/}
                      {/*1 of 3*/}
                  {/*</div>*/}
                  {/*<div className="col">*/}
                      {/*2 of 3*/}
                  {/*</div>*/}
                  {/*<div className="col">*/}
                      {/*3 of 3*/}
                  {/*</div>*/}
              {/*</div>*/}
          {/*</div>*/}
      </div>





    );
  }
}

export default App;
