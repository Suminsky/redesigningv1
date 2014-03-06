#pragma once
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
	created:	2014/02/22
	created:	22:2:2014   20:14
	file:		OpenDialog.h
	author:		Icebone1000 (Giuliano Suminsky Pieta)
	
	purpose:	

	© Icebone1000 (Giuliano Suminsky Pieta) , rights reserved.
*/
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// standard includes
#include <vector>
#include <string>

// shell stuff
#include <Windows.h>
#include <shlobj.h>
#define STRICT_TYPED_ITEMIDS
#include <shlwapi.h>
#include <objbase.h>      // For COM headers
#include <shobjidl.h>     // for IFileDialogEvents and IFileDialogControlEvents
#include <knownfolders.h> // for KnownFolder APIs/datatypes/function headers
#include <propvarutil.h>  // for PROPVAR-related functions
#include <propkey.h>      // for the Property key APIs/datatypes
#include <propidl.h>      // for the Property System APIs
#include <strsafe.h>      // for StringCchPrintfW
#include <shtypes.h>      // for COMDLG_FILTERSPEC

#pragma comment(linker, "\"/manifestdependency:type='Win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment( lib, "propsys.lib" )
#pragma comment( lib, "shlwapi.lib" )
#pragma comment( lib, "comctl32.lib" )

// private includes

namespace win{

	//------------------------------------------------------------------------
	// file types given as:
	// const COMDLG_FILTERSPEC c_rgSaveTypes[] =
	// {
	// 	{L"PNG",       L"*.png"}
	// };
	// the first one will be used as the default
	// 
	// Note that it returns HRESULT_FROM_WIN32(ERROR_CANCELLED) if user cancels
	//------------------------------------------------------------------------
	inline static HRESULT OpenDialog( std::vector<std::wstring> & filePaths_p, const COMDLG_FILTERSPEC pFileTypes_p[], DWORD nTypes_p, bool bMulti_p ){

		// CoCreate the File Open Dialog object.

		IFileOpenDialog *pfd = NULL;
		HRESULT hr = CoCreateInstance( CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd) );
		if (SUCCEEDED(hr)){

			if (SUCCEEDED(hr)){

				// Set the options on the dialog.

				DWORD dwFlags;

				// Before setting, always get the options first in order not to override existing options.

				hr = pfd->GetOptions( &dwFlags );
				if (SUCCEEDED(hr)){

					// In this case, get shell items only for file system items.

					hr = pfd->SetOptions( dwFlags | FOS_FORCEFILESYSTEM | (bMulti_p ? FOS_ALLOWMULTISELECT : 0) );
					if (SUCCEEDED(hr)){

						// Set the file types to display only. Notice that, this is a 1-based array.

						hr = pfd->SetFileTypes( nTypes_p, pFileTypes_p );
						if (SUCCEEDED(hr)){

							// Set the selected file type index to Word Docs for this example.

							hr = pfd->SetFileTypeIndex(0);
							if (SUCCEEDED(hr)){

								// Set the default extension to be ".doc" file.

								hr = pfd->SetDefaultExtension( pFileTypes_p[0].pszName ); //L"png");
								if (SUCCEEDED(hr)){

									// Show the dialog
									hr = pfd->Show(NULL);
									if (SUCCEEDED(hr)){

										// Obtain the result, once the user clicks the 'Open' button.
										// The result is an IShellItem object.

										IShellItemArray *psiResults;
										hr = pfd->GetResults( &psiResults );
										if (SUCCEEDED(hr)){

											// We are just going to print out the name of the file for sample sake.

											DWORD count;
											hr = psiResults->GetCount( &count );

											if( SUCCEEDED(hr) )
											for( DWORD it = 0; it < count; ++ it ){

												IShellItem *psiResult;
												hr = psiResults->GetItemAt( it, &psiResult );

												if( SUCCEEDED(hr) ){

													PWSTR pszFilePath = NULL;

													hr = psiResult->GetDisplayName( SIGDN_FILESYSPATH, &pszFilePath );

													filePaths_p.push_back( pszFilePath );

													psiResult->Release();
												}
											}// for

											psiResults->Release();
										}
									}
								}
							}
						}
					}
				}
			}

			pfd->Release();
		}

		return hr;
	}




	inline static HRESULT SaveDialog( std::wstring & filePath_p, const COMDLG_FILTERSPEC pFileTypes_p[], DWORD nTypes_p, BOOL & bSequentialFilesOpt_p ){

		// CoCreate the File Save Dialog object.

		IFileSaveDialog *pfd = nullptr;
		HRESULT hr = CoCreateInstance( CLSID_FileSaveDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd) );

		// accessed when creating and later when checking the results

		IFileDialogCustomize *pfdc = nullptr;
		DWORD ctrlGroupID = 2000;

		if (SUCCEEDED(hr)){

			// add check box for saving a sequence adding a suffix to the end of file
			if( bSequentialFilesOpt_p ){

				hr = pfd->QueryInterface( IID_PPV_ARGS(&pfdc) );
				if (SUCCEEDED(hr)){

					// Create a Visual Group.
					
					hr = pfdc->StartVisualGroup(ctrlGroupID, L"Options");
					if (SUCCEEDED(hr)){

						hr = pfdc->AddCheckButton( ctrlGroupID+1, L"Save all files with that name (name_01, name_02..)", false );
						if (SUCCEEDED(hr)){

							hr = pfdc->SetControlState( 0, CDCS_VISIBLE | CDCS_ENABLED );
						}
					}

					hr = pfdc->EndVisualGroup();
				}

				//pfdc->Release();
			}

			if( SUCCEEDED(hr) ){

				// Set the options on the dialog.

				DWORD dwFlags;

				// Before setting, always get the options first in order not to override existing options.

				hr = pfd->GetOptions( &dwFlags );
				if (SUCCEEDED(hr)){

					// In this case, get shell items only for file system items.

					hr = pfd->SetOptions( dwFlags | FOS_FORCEFILESYSTEM );
					if (SUCCEEDED(hr)){

						// Set the file types to display only. Notice that, this is a 1-based array.

						hr = pfd->SetFileTypes( nTypes_p, pFileTypes_p );
						if (SUCCEEDED(hr)){

							// Set the selected file type index to Word Docs for this example.

							hr = pfd->SetFileTypeIndex(0);
							if (SUCCEEDED(hr)){

								// Set the default extension to be ".doc" file.

								hr = pfd->SetDefaultExtension( pFileTypes_p[0].pszName ); //L"png");
								if (SUCCEEDED(hr)){

									// Show the dialog
									hr = pfd->Show(NULL);
									if (SUCCEEDED(hr)){

										// Obtain the result, once the user clicks the 'Open' button.
										// The result is an IShellItem object.

										IShellItem *psiResult;
										hr = pfd->GetResult( &psiResult );
										if (SUCCEEDED(hr)){

											PWSTR pszFilePath = NULL;

											hr = psiResult->GetDisplayName( SIGDN_FILESYSPATH, &pszFilePath );
											
											filePath_p = pszFilePath;

											psiResult->Release();

											if( bSequentialFilesOpt_p ){

												hr = pfdc->GetCheckButtonState( ctrlGroupID+1, &bSequentialFilesOpt_p );

												pfdc->Release();
											}
										}
									}
								}
							}
						}
					}
				}

			}

			pfd->Release();
		}

		return hr;
	}
}