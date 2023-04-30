//
//  ContentView.swift
//  MidExam3
//
//  Created by 민시온 on 2023/04/21.
//

import SwiftUI

struct ContentView: View {
    @State private var showingPopover = false
    @State private var showingSheet = false

var body: some View {
  VStack {
    Text("2019112635 민시온").font(.largeTitle).foregroundColor(.blue)
    Spacer()
    Button(action: { self.showingSheet.toggle() }) {
    Text("Sheet")
      .font(.largeTitle).foregroundColor(.blue)
    }
    .sheet(isPresented: $showingSheet,
         onDismiss: {print("Dismissed")}, // 화면이 닫힌 후  수행할 작업
         content: { PresentedView(isPresented: $showingSheet) } // 새로 출력될 화면
    )
    
    Button(action: { self.showingPopover.toggle() }) {
    Text("Popover").font(.largeTitle)
    }
    .popover(isPresented: $showingPopover,
       attachmentAnchor: .rect(.bounds), // (1)
       arrowEdge: .top, // (2)
       content: popoverContents
    )
    Spacer()
    }
}

    
    func popoverContents() -> some View { // 팝오버에 표시될 컨텐츠
    VStack(alignment: .leading) {
      HStack {
        Button(action: {self.showingPopover = false}) { // 팝오버 제거
            Text("Cancel").foregroundColor(.red).cornerRadius(10)
        }
        Spacer()
        Text("New Event").font(.headline)
        Spacer()
        Button("Add(+)") { }
      }
      Divider().padding(.bottom, 8) // 구분선 추가
      
      Text("Title")
      TextField("제목", text: .constant(""))
      Text("Contents")
      TextField("내용", text: .constant(""))
      Spacer()
    }
    .padding()
    }
}

struct PresentedView: View {
  @Binding var isPresented: Bool
    let fruits = ["사과", "배", "포도", "바나나"]
    let drinks = ["물", "우유", "탄산수"]
  var body: some View {
      let titles = ["Fruits", "Drinks"]
      let data = [fruits, drinks]

      List {
          Button(action: { self.isPresented = false }) {
            Text("Tap to Dismiss")
                  .font(.title).foregroundColor(.blue)
          }
        ForEach(data.indices, id: \.self) { index in  // data에 포함된 횟수만큼 섹션 생성
          Section(header: Text(titles[index]).font(.title),   // 헤더
                  footer: HStack{ Spacer(); Text("\(data[index].count)건")} // 푸터
          ) { // 콘텐츠
            ForEach(data[index], id: \.self) {
              Text($0)
            }
          } // Section
        } // ForEach
      } // List
    }
  }



struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
