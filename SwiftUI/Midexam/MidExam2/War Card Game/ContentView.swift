//
//  ContentView.swift
//  War Card Game
//
//  Mobile App Programming2 - MidExam2
//

import SwiftUI

struct ContentView: View {
  
  @State var playerCard = "card7"
  @State var cpuCard = "card13"
  
  @State var playerScore = 0
  @State var cpuScore =  0
  
  var body: some View {
    ZStack {
      Image("background-plain")
        .resizable()
        .ignoresSafeArea()
      VStack {
        Spacer()
        // Replace with your student number and name.
        Text("2019112635 민시온")
          .font(.largeTitle).foregroundColor(.white)
        Image("logo")
        Spacer()
        HStack {
          Spacer()
          Image(playerCard)
          Spacer()
          Image(cpuCard)
          Spacer()
        }
        Spacer()
        Button {
          deal()
        } label: {
          Image("button")
        }
        Spacer()
        HStack {
          Spacer()
          VStack {
            Text("Player")
              .font(.headline)
              .padding(.bottom, 10.0)
            Text(String(playerScore))
              .font(.largeTitle)
          }
          Spacer()
          VStack {
            Text("CPU")
              .font(.headline)
              .padding(.bottom, 10.0)
            Text(String(cpuScore))
              .font(.largeTitle)
          }
          Spacer()
        }
        .foregroundColor(.white)
        Spacer()
      }
      
    }
  }
  
  // Implement this function as the downloaded gif file works.
  func deal() {
    // Randomize the players card
    let rand1 = Int.random(in: 2...14)
    self.playerCard = "card\(rand1)"
    
    // Randomize the cpus cards
    let rand2 = Int.random(in: 2...14)
    self.cpuCard = "card\(rand2)"

    // Update the scores
      if rand1 > rand2 && self.playerCard != "card14" {
          self.playerScore += 1
      }
      if rand1 < rand2 && self.cpuCard != "card14"{
          self.cpuScore += 1
      }
    
  }
  
}

struct ContentView_Previews: PreviewProvider {
  static var previews: some View {
    ContentView()
  }
}
