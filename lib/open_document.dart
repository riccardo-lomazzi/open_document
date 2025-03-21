import 'dart:async';
import 'open_document_platform_interface.dart';

///- Serves to create a local folder with the name of the app
///- Storage of documents and files
///- It has the power to open the zip creating a new folder with
/// the name of the file using the plugin [Archive]
class OpenDocument {
  /// - Open the document by the indicated path [filePath],
  static Future<void> openDocument({required String filePath}) async {
    return await OpenDocumentPlatform.instance.openDocument(filePath: filePath);
  }

  /// - Return path folder
  static Future<String> getPathDocument() async {
    return await OpenDocumentPlatform.instance.getPathDocument();
  }

  /// - Get the folder name is the same as the application
  static Future<String> getNameFolder() async {
    return await OpenDocumentPlatform.instance.getNameFolder();
  }

  /// - Get the url name
  static Future<String> getNameFile({required String url}) async {
    return await OpenDocumentPlatform.instance.getNameFile(url: url);
  }

  /// - Check if the path already exists in document folder [filePath]
  static checkDocument({required String filePath}) {
    return OpenDocumentPlatform.instance.checkDocument(filePath: filePath);
  }
}
