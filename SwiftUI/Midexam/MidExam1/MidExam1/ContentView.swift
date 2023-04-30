//
//  ContentView.swift
//  MidExam1
//
//  Created by 민시온 on 2023/04/21.
//

import SwiftUI

struct ContentView: View {
    var body: some View {
        ZStack(){
            Color.clear
                VStack(alignment: .leading) {
                    Image("bts").resizable().cornerRadius(21)
                    Spacer(minLength: 15)
                    HStack {
                        Text("BTS")
                            .font(.title)
                            .fontWeight(.bold)
                        Spacer(minLength: 15)
                        VStack {
                            HStack {
                                Image(systemName: "star.fill")
                                Image(systemName: "star.fill")
                                Image(systemName: "star.fill")
                                Image(systemName: "star.fill")
                                Image(systemName: "star.leadinghalf.filled")
                            }
                            Text("(Reviews 361)")
                        }
                        .font(/*@START_MENU_TOKEN@*/.footnote/*@END_MENU_TOKEN@*/)
                        .foregroundColor(Color.orange)
                        
                    }
                    Spacer(minLength: 15)
                    Text("Sing together with BTS")
                    Spacer(minLength: 15)
                    HStack {
                        Text("<2019112635 민시온>")
                        Spacer(minLength: 15)
                        Image(systemName: "fork.knife")
                        Image(systemName: "binoculars.fill")
                    }
                }
                .frame(width: 350, height: 300)
                .padding(20)
                .background(Color.white)
                .cornerRadius(21)
                .shadow(color: Color.black.opacity(0.2), radius: 10, x: 0, y: -5)
            
        }.background(Color(UIColor.systemYellow))
        
    }
}


struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
