/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2010  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PrecompiledHeader.h"
#include "App.h"
#include "Plugins.h"

#include "MemoryCardFile.h"
#include "Utilities/IniInterface.h"

#include <wx/stdpaths.h>
#include "DebugTools/Debug.h"
#include <memory>


#include "options_tools.h"
//////////////////////////////////////////////////////////////////////////////////////////
// PathDefs Namespace -- contains default values for various pcsx2 path names and locations.
//
// Note: The members of this namespace are intended for default value initialization only.
// Most of the time you should use the path folder assignments in Conf() instead, since those
// are user-configurable.
//
namespace PathDefs
{
	namespace Base
	{
		const wxDirName& Snapshots()
		{
			static const wxDirName retval( L"snaps" );
			return retval;
		}

		const wxDirName& Savestates()
		{
			static const wxDirName retval( L"sstates" );
			return retval;
		}

		const wxDirName& MemoryCards()
		{
			static const wxDirName retval( L"memcards" );
			return retval;
		}

		const wxDirName& Settings()
		{
			static const wxDirName retval( L"inis" );
			return retval;
		}

		const wxDirName& Plugins()
		{
			static const wxDirName retval( L"plugins" );
			return retval;
		}

		const wxDirName& Logs()
		{
			static const wxDirName retval( L"logs" );
			return retval;
		}

		const wxDirName& Bios()
		{
			static const wxDirName retval(L"bios");
			return retval;
		}

		const wxDirName& Cheats()
		{
			static const wxDirName retval(L"cheats");
			return retval;
		}

		const wxDirName& CheatsWS()
		{
			static const wxDirName retval(L"cheats_ws");
			return retval;
		}

		const wxDirName& Langs()
		{
			static const wxDirName retval( L"Langs" );
			return retval;
		}

		const wxDirName& Dumps()
		{
			static const wxDirName retval( L"dumps" );
			return retval;
		}
		
		const wxDirName& Docs()
		{
			static const wxDirName retval( L"docs" );
			return retval;
		}
	};

	// Specifies the root folder for the application install.
	// (currently it's the CWD, but in the future I intend to move all binaries to a "bin"
	// sub folder, in which case the approot will become "..") [- Air?]

	//The installer installs the folders which are relative to AppRoot (that's plugins/langs)
	//  relative to the exe folder, and not relative to cwd. So the exe should be default AppRoot. - avih
	const wxDirName& AppRoot()
	{
		//AffinityAssert_AllowFrom_MainUI();
/*
		if (InstallationMode == InstallMode_Registered)
		{
			static const wxDirName cwdCache( (wxDirName)Path::Normalize(wxGetCwd()) );
			return cwdCache;
		}
		else if (InstallationMode == InstallMode_Portable)
*/		
		if (InstallationMode == InstallMode_Registered || InstallationMode == InstallMode_Portable)
		{
			static const wxDirName appCache( (wxDirName)
				wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath() );
			return appCache;
		}
		else
			pxFail( "Unimplemented user local folder mode encountered." );
		
		static const wxDirName dotFail(L".");
		return dotFail;
	}

    // Specifies the main configuration folder.
    wxDirName GetUserLocalDataDir()
    {
        return wxDirName(wxStandardPaths::Get().GetUserLocalDataDir());
    }

	// Fetches the path location for user-consumable documents -- stuff users are likely to want to
	// share with other programs: screenshots, memory cards, and savestates.
	wxDirName GetDocuments( DocsModeType mode )
	{
		switch( mode )
		{
#ifdef XDG_STD
			// Move all user data file into central configuration directory (XDG_CONFIG_DIR)
			case DocsFolder_User:	return GetUserLocalDataDir();
#else
			case DocsFolder_User:	return (wxDirName)Path::Combine( wxStandardPaths::Get().GetDocumentsDir(), pxGetAppName() );
#endif
			case DocsFolder_Custom: return CustomDocumentsFolder;

			jNO_DEFAULT
		}

		return wxDirName();
	}

	wxDirName GetDocuments()
	{
		return GetDocuments( DocsFolderMode );
	}

	wxDirName GetProgramDataDir()
	{
#ifndef GAMEINDEX_DIR_COMPILATION
		return AppRoot();
#else
		// Each linux distributions have his rules for path so we give them the possibility to
		// change it with compilation flags. -- Gregory
#define xGAMEINDEX_str(s) GAMEINDEX_DIR_str(s)
#define GAMEINDEX_DIR_str(s) #s
		return wxDirName( xGAMEINDEX_str(GAMEINDEX_DIR_COMPILATION) );
#endif
	}

	wxDirName GetSnapshots()
	{
		return GetDocuments() + Base::Snapshots();
	}

	wxDirName GetBios()
	{
		return GetDocuments() + Base::Bios();;
	}

	wxDirName GetCheats()
	{
		return GetDocuments() + Base::Cheats();
	}

	wxDirName GetCheatsWS()
	{
		return GetDocuments() + Base::CheatsWS();
	}
	
	wxDirName GetDocs()
	{
		return AppRoot() + Base::Docs();
	}

	wxDirName GetSavestates()
	{
		return GetDocuments() + Base::Savestates();
	}

	wxDirName GetMemoryCards()
	{
		return GetDocuments() + Base::MemoryCards();
	}

	wxDirName GetPlugins()
	{
		// Each linux distributions have his rules for path so we give them the possibility to
		// change it with compilation flags. -- Gregory
#ifndef PLUGIN_DIR_COMPILATION
		return AppRoot() + Base::Plugins();
#else
#define xPLUGIN_DIR_str(s) PLUGIN_DIR_str(s)
#define PLUGIN_DIR_str(s) #s
		return wxDirName( xPLUGIN_DIR_str(PLUGIN_DIR_COMPILATION) );
#endif
	}

	wxDirName GetSettings()
	{
		return GetDocuments() + Base::Settings();
	}

	wxDirName GetLogs()
	{
		return GetDocuments() + Base::Logs();
	}

	wxDirName GetLangs()
	{
		return AppRoot() + Base::Langs();
	}

	wxDirName Get( FoldersEnum_t folderidx )
	{
		switch( folderidx )
		{
			case FolderId_Plugins:		return GetPlugins();
			case FolderId_Settings:		return GetSettings();
			case FolderId_Bios:			return GetBios();
			case FolderId_Snapshots:	return GetSnapshots();
			case FolderId_Savestates:	return GetSavestates();
			case FolderId_MemoryCards:	return GetMemoryCards();
			case FolderId_Logs:			return GetLogs();
			case FolderId_Langs:		return GetLangs();
			case FolderId_Cheats:		return GetCheats();
			case FolderId_CheatsWS:		return GetCheatsWS();

			case FolderId_Documents:	return CustomDocumentsFolder;

			jNO_DEFAULT
		}
		return wxDirName();
	}
};

wxDirName& AppConfig::FolderOptions::operator[]( FoldersEnum_t folderidx )
{
	switch( folderidx )
	{
		case FolderId_Plugins:		return PluginsFolder;
		case FolderId_Settings:		return SettingsFolder;
		case FolderId_Bios:			return Bios;
		case FolderId_Snapshots:	return Snapshots;
		case FolderId_Savestates:	return Savestates;
		case FolderId_MemoryCards:	return MemoryCards;
		case FolderId_Logs:			return Logs;
		case FolderId_Langs:		return Langs;
		case FolderId_Cheats:		return Cheats;
		case FolderId_CheatsWS:		return CheatsWS;

		case FolderId_Documents:	return CustomDocumentsFolder;

		jNO_DEFAULT
	}
	return PluginsFolder;		// unreachable, but suppresses warnings.
}

const wxDirName& AppConfig::FolderOptions::operator[]( FoldersEnum_t folderidx ) const
{
	return const_cast<FolderOptions*>( this )->operator[]( folderidx );
}

bool AppConfig::FolderOptions::IsDefault( FoldersEnum_t folderidx ) const
{
	switch( folderidx )
	{
		case FolderId_Plugins:		return UseDefaultPluginsFolder;
		case FolderId_Settings:		return UseDefaultSettingsFolder;
		case FolderId_Bios:			return UseDefaultBios;
		case FolderId_Snapshots:	return UseDefaultSnapshots;
		case FolderId_Savestates:	return UseDefaultSavestates;
		case FolderId_MemoryCards:	return UseDefaultMemoryCards;
		case FolderId_Logs:			return UseDefaultLogs;
		case FolderId_Langs:		return UseDefaultLangs;
		case FolderId_Cheats:		return UseDefaultCheats;
		case FolderId_CheatsWS:		return UseDefaultCheatsWS;

		case FolderId_Documents:	return false;

		jNO_DEFAULT
	}
	return false;
}

void AppConfig::FolderOptions::Set( FoldersEnum_t folderidx, const wxString& src, bool useDefault )
{
	switch( folderidx )
	{
		case FolderId_Plugins:
			PluginsFolder = src;
			UseDefaultPluginsFolder = useDefault;
		break;

		case FolderId_Settings:
			SettingsFolder = src;
			UseDefaultSettingsFolder = useDefault;
		break;

		case FolderId_Bios:
			Bios = src;
			UseDefaultBios = useDefault;
		break;

		case FolderId_Snapshots:
			Snapshots = src;
			UseDefaultSnapshots = useDefault;
		break;

		case FolderId_Savestates:
			Savestates = src;
			UseDefaultSavestates = useDefault;
		break;

		case FolderId_MemoryCards:
			MemoryCards = src;
			UseDefaultMemoryCards = useDefault;
		break;

		case FolderId_Logs:
			Logs = src;
			UseDefaultLogs = useDefault;
		break;

		case FolderId_Langs:
			Langs = src;
			UseDefaultLangs = useDefault;
		break;

		case FolderId_Documents:
			CustomDocumentsFolder = src;
		break;

		case FolderId_Cheats:
			Cheats = src;
			UseDefaultCheats = useDefault;
		break;

		case FolderId_CheatsWS:
			CheatsWS = src;
			UseDefaultCheatsWS = useDefault;
		break;

		jNO_DEFAULT
	}
}

// --------------------------------------------------------------------------------------
//  Default Filenames
// --------------------------------------------------------------------------------------
namespace FilenameDefs
{
	wxFileName GetUiConfig()
	{
		return pxGetAppName() + L"_ui.ini";
	}

	wxFileName GetUiKeysConfig()
	{
		return pxGetAppName() + L"_keys.ini";
	}

	wxFileName GetVmConfig()
	{
		return pxGetAppName() + L"_vm.ini";
	}

	wxFileName GetUsermodeConfig()
	{
		return wxFileName( L"usermode.ini" );
	}

	const wxFileName& Memcard( uint port, uint slot )
	{
		static const wxFileName retval[2][4] =
		{
			{
				wxFileName( L"Mcd001.ps2" ),
				wxFileName( L"Mcd003.ps2" ),
				wxFileName( L"Mcd005.ps2" ),
				wxFileName( L"Mcd007.ps2" ),
			},
			{
				wxFileName( L"Mcd002.ps2" ),
				wxFileName( L"Mcd004.ps2" ),
				wxFileName( L"Mcd006.ps2" ),
				wxFileName( L"Mcd008.ps2" ),
			}
		};

		IndexBoundsAssumeDev( L"FilenameDefs::Memcard", port, 2 );
		IndexBoundsAssumeDev( L"FilenameDefs::Memcard", slot, 4 );

		return retval[port][slot];
	}
};

wxString AppConfig::FullpathTo( PluginsEnum_t pluginidx ) const
{
	return Path::Combine( PluginsFolder, BaseFilenames[pluginidx] );
}

// returns true if the filenames are quite absolutely the equivalent.  Works for all
// types of filenames, relative and absolute.  Very important that you use this function
// rather than any other type of more direct string comparison!
bool AppConfig::FullpathMatchTest( PluginsEnum_t pluginId, const wxString& cmpto ) const
{
	// Implementation note: wxFileName automatically normalizes things as needed in it's
	// equality comparison implementations, so we can do a simple comparison as follows:

	return wxFileName(cmpto).SameAs( FullpathTo(pluginId) );
}

static wxDirName GetResolvedFolder(FoldersEnum_t id)
{
	return g_Conf->Folders.IsDefault(id) ? PathDefs::Get(id) : g_Conf->Folders[id];
}

wxDirName GetLogFolder()
{
	return GetResolvedFolder(FolderId_Logs);
}

wxDirName GetCheatsFolder()
{
	return GetResolvedFolder(FolderId_Cheats);
}

wxDirName GetCheatsWsFolder()
{
	return GetResolvedFolder(FolderId_CheatsWS);
}

wxDirName GetSettingsFolder()
{
	if( wxGetApp().Overrides.SettingsFolder.IsOk() )
		return wxGetApp().Overrides.SettingsFolder;

	return UseDefaultSettingsFolder ? PathDefs::GetSettings() : SettingsFolder;
}

wxString GetVmSettingsFilename()
{
	wxFileName fname( wxGetApp().Overrides.VmSettingsFile.IsOk() ? wxGetApp().Overrides.VmSettingsFile : FilenameDefs::GetVmConfig() );
	return GetSettingsFolder().Combine( fname ).GetFullPath();
}

wxString GetUiSettingsFilename()
{
	wxFileName fname( FilenameDefs::GetUiConfig() );
	return GetSettingsFolder().Combine( fname ).GetFullPath();
}

wxString GetUiKeysFilename()
{
	wxFileName fname( FilenameDefs::GetUiKeysConfig() );
	return GetSettingsFolder().Combine( fname ).GetFullPath();
}


wxString AppConfig::FullpathToBios() const				{ return Path::Combine( Folders.Bios, BaseFilenames.Bios ); }
wxString AppConfig::FullpathToMcd( uint slot ) const
{
	return Path::Combine( Folders.MemoryCards, Mcd[slot].Filename );
}

bool IsPortable()
{
	return InstallationMode==InstallMode_Portable;
}

AppConfig::AppConfig()
	: SysSettingsTabName( L"Cpu" )
	, McdSettingsTabName( L"none" )
	, ComponentsTabName( L"Plugins" )
	, AppSettingsTabName( L"none" )
	, GameDatabaseTabName( L"none" )
{
	LanguageId			= wxLANGUAGE_DEFAULT;
	LanguageCode		= L"default";
	RecentIsoCount		= 20;
	Listbook_ImageSize	= 32;
	Toolbar_ImageSize	= 24;
	Toolbar_ShowLabels	= true;

	#ifdef __WXMSW__
	McdCompressNTFS		= true;
	#endif
	EnableSpeedHacks	= true;
	EnableGameFixes		= false;
	EnableFastBoot		= true;

	EnablePresets		= true;
	PresetIndex			= 1;

	CdvdSource			= CDVD_SourceType::Iso;

	// To be moved to FileMemoryCard pluign (someday)
	for( uint slot=0; slot<8; ++slot )
	{
		Mcd[slot].Enabled	= !FileMcd_IsMultitapSlot(slot);	// enables main 2 slots
		Mcd[slot].Filename	= FileMcd_GetDefaultName( slot );

		// Folder memory card is autodetected later.
		Mcd[slot].Type = MemoryCardType::MemoryCard_File;
	}

	GzipIsoIndexTemplate = L"$(f).pindex.tmp";
}

// ------------------------------------------------------------------------
void App_LoadSaveInstallSettings( IniInterface& ini )
{
	// Portable installs of PCSX2 should not save any of the following information to
	// the INI file.  Only the Run First Time Wizard option is saved, and that's done
	// from EstablishAppUserMode code.  All other options have assumed (fixed) defaults in
	// portable mode which cannot be changed/saved.

	// Note: Settins are still *loaded* from portable.ini, in case the user wants to do
	// low-level overrides of the default behavior of portable mode installs.

	if (ini.IsSaving() && (InstallationMode == InstallMode_Portable)) return;

	static const wxChar* DocsFolderModeNames[] =
	{
		L"User",
		L"Custom",
		// WARNING: array must be NULL terminated to compute it size
		NULL
	};

	ini.EnumEntry( L"DocumentsFolderMode",	DocsFolderMode,	DocsFolderModeNames, (InstallationMode == InstallMode_Registered) ? DocsFolder_User : DocsFolder_Custom);

	ini.Entry( L"CustomDocumentsFolder",	CustomDocumentsFolder,		PathDefs::AppRoot() );

	ini.Entry( L"UseDefaultSettingsFolder", UseDefaultSettingsFolder,	true );
	ini.Entry( L"SettingsFolder",			SettingsFolder,				PathDefs::GetSettings() );

	// "Install_Dir" conforms to the NSIS standard install directory key name.
	// Attempt to load plugins based on the Install Folder.

	ini.Entry( L"Install_Dir",				InstallFolder,				(wxDirName)(wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath()) );
	SetFullBaseDir( InstallFolder );

	//ini.Entry( L"PluginsFolder",			PluginsFolder,				InstallFolder + PathDefs::Base::Plugins() );

	ini.Flush();
}

void App_LoadInstallSettings( wxConfigBase* ini )
{
	IniLoader loader( ini );
	App_LoadSaveInstallSettings( loader );
}

void App_SaveInstallSettings( wxConfigBase* ini )
{
	IniSaver saver( ini );
	App_LoadSaveInstallSettings( saver );
}

// ------------------------------------------------------------------------
void AppConfig::LoadSaveMemcards( IniInterface& ini )
{
	ScopedIniGroup path( ini, L"MemoryCards" );

	for( uint slot=0; slot<2; ++slot )
	{
		ini.Entry( pxsFmt( L"Slot%u_Enable", slot+1 ),
			Mcd[slot].Enabled, Mcd[slot].Enabled );
		ini.Entry( pxsFmt( L"Slot%u_Filename", slot+1 ),
			Mcd[slot].Filename, Mcd[slot].Filename );
	}

	for( uint slot=2; slot<8; ++slot )
	{
		int mtport = FileMcd_GetMtapPort(slot)+1;
		int mtslot = FileMcd_GetMtapSlot(slot)+1;

		ini.Entry( pxsFmt( L"Multitap%u_Slot%u_Enable", mtport, mtslot ),
			Mcd[slot].Enabled, Mcd[slot].Enabled );
		ini.Entry( pxsFmt( L"Multitap%u_Slot%u_Filename", mtport, mtslot ),
			Mcd[slot].Filename, Mcd[slot].Filename );
	}
}

void AppConfig::LoadSaveRootItems( IniInterface& ini )
{
	IniEntry( SysSettingsTabName );
	IniEntry( McdSettingsTabName );
	IniEntry( ComponentsTabName );
	IniEntry( AppSettingsTabName );
	IniEntry( GameDatabaseTabName );
	ini.EnumEntry( L"LanguageId", LanguageId, NULL, LanguageId );
	IniEntry( LanguageCode );
	IniEntry( RecentIsoCount );
	IniEntry( GzipIsoIndexTemplate );
	IniEntry( Listbook_ImageSize );
	IniEntry( Toolbar_ImageSize );
	IniEntry( Toolbar_ShowLabels );

	wxFileName res(CurrentIso);
	ini.Entry( L"CurrentIso", res, res, ini.IsLoading() || IsPortable() );
	CurrentIso = res.GetFullPath();

	IniEntry( CurrentBlockdump );
	IniEntry( CurrentELF );
	IniEntry( CurrentIRX );

	IniEntry( EnableSpeedHacks );
	IniEntry( EnableGameFixes );
	IniEntry( EnableFastBoot );

	IniEntry( EnablePresets );
	IniEntry( PresetIndex );
	IniEntry( AskOnBoot );
	
	#ifdef __WXMSW__
	IniEntry( McdCompressNTFS );
	#endif

	ini.EnumEntry( L"CdvdSource", CdvdSource, CDVD_SourceLabels, CdvdSource );
}

// ------------------------------------------------------------------------
void AppConfig::LoadSave( IniInterface& ini )
{
	LoadSaveRootItems( ini );
	LoadSaveMemcards( ini );

	// Process various sub-components:
	Folders			.LoadSave( ini );
	BaseFilenames	.LoadSave( ini );
	GSWindow		.LoadSave( ini );
	Framerate		.LoadSave( ini );
	Templates		.LoadSave( ini );

	ini.Flush();
}
void AppConfig::FolderOptions::ApplyDefaults()
{
	if( UseDefaultBios )		Bios		  = PathDefs::GetBios();
	if( UseDefaultSnapshots )	Snapshots	  = PathDefs::GetSnapshots();
	if( UseDefaultSavestates )	Savestates	  = PathDefs::GetSavestates();
	if( UseDefaultMemoryCards )	MemoryCards	  = PathDefs::GetMemoryCards();
	if( UseDefaultLogs )		Logs		  = PathDefs::GetLogs();
	if( UseDefaultLangs )		Langs		  = PathDefs::GetLangs();
	if( UseDefaultPluginsFolder)PluginsFolder = PathDefs::GetPlugins();
	if( UseDefaultCheats )      Cheats		  = PathDefs::GetCheats();
	if( UseDefaultCheatsWS )    CheatsWS	  = PathDefs::GetCheatsWS();
}

// ------------------------------------------------------------------------
AppConfig::FolderOptions::FolderOptions()
	: Bios			( PathDefs::GetBios() )
	, Snapshots		( PathDefs::GetSnapshots() )
	, Savestates	( PathDefs::GetSavestates() )
	, MemoryCards	( PathDefs::GetMemoryCards() )
	, Langs			( PathDefs::GetLangs() )
	, Logs			( PathDefs::GetLogs() )
	, Cheats		( PathDefs::GetCheats() )
	, CheatsWS      ( PathDefs::GetCheatsWS() )

	, RunIso	( PathDefs::GetDocuments() )			// raw default is always the Documents folder.
	, RunELF	( PathDefs::GetDocuments() )			// raw default is always the Documents folder.
	, RunDisc	( PathDefs::GetDocuments().GetFilename() )
{
	bitset = 0xffffffff;
}

void AppConfig::FolderOptions::LoadSave( IniInterface& ini )
{
	ScopedIniGroup path( ini, L"Folders" );

	if( ini.IsSaving() )
	{
		ApplyDefaults();
	}

	IniBitBool( UseDefaultBios );
	IniBitBool( UseDefaultSnapshots );
	IniBitBool( UseDefaultSavestates );
	IniBitBool( UseDefaultMemoryCards );
	IniBitBool( UseDefaultLogs );
	IniBitBool( UseDefaultLangs );
	IniBitBool( UseDefaultPluginsFolder );
	IniBitBool( UseDefaultCheats );
	IniBitBool( UseDefaultCheatsWS );

	//when saving in portable mode, we save relative paths if possible
	 //  --> on load, these relative paths will be expanded relative to the exe folder.
	bool rel = ( ini.IsLoading() || IsPortable() );
	
	IniEntryDirFile( Bios,  rel);
	IniEntryDirFile( Snapshots,  rel );
	IniEntryDirFile( Savestates,  rel );
	IniEntryDirFile( MemoryCards,  rel );
	IniEntryDirFile( Logs,  rel );
	IniEntryDirFile( Langs,  rel );
	IniEntryDirFile( Cheats, rel );
	IniEntryDirFile( CheatsWS, rel );
	ini.Entry( L"PluginsFolder", PluginsFolder, InstallFolder + PathDefs::Base::Plugins(), rel );

	IniEntryDirFile( RunIso, rel );
	IniEntryDirFile( RunELF, rel );
	IniEntryDirFile( RunDisc, rel );

	if( ini.IsLoading() )
	{
		ApplyDefaults();

		for( int i=0; i<FolderId_COUNT; ++i )
			operator[]( (FoldersEnum_t)i ).Normalize();
	}
}

// ------------------------------------------------------------------------
const wxFileName& AppConfig::FilenameOptions::operator[]( PluginsEnum_t pluginidx ) const
{
	IndexBoundsAssumeDev( L"Filename[Plugin]", pluginidx, PluginId_Count );
	return Plugins[pluginidx];
}

void AppConfig::FilenameOptions::LoadSave( IniInterface& ini )
{
	ScopedIniGroup path( ini, L"Filenames" );

	static const wxFileName pc( L"Please Configure" );

	//when saving in portable mode, we just save the non-full-path filename
 	//  --> on load they'll be initialized with default (relative) paths (works both for plugins and bios)
	//note: this will break if converting from install to portable, and custom folders are used. We can live with that.
	bool needRelativeName = ini.IsSaving() && IsPortable();

	for( int i=0; i<PluginId_Count; ++i )
	{
		if ( needRelativeName ) {
			wxFileName plugin_filename = wxFileName( Plugins[i].GetFullName() );
			ini.Entry( tbl_PluginInfo[i].GetShortname(), plugin_filename, pc );
		} else
			ini.Entry( tbl_PluginInfo[i].GetShortname(), Plugins[i], pc );
	}

	if( needRelativeName ) { 
		wxFileName bios_filename = wxFileName( Bios.GetFullName() );
		ini.Entry( L"BIOS", bios_filename, pc );
	} else
		ini.Entry( L"BIOS", Bios, pc );
}

// ------------------------------------------------------------------------
AppConfig::GSWindowOptions::GSWindowOptions()
{
	CloseOnEsc				= true;
	DefaultToFullscreen		= false;
	AlwaysHideMouse			= false;
	DisableResizeBorders	= false;
	DisableScreenSaver		= true;

	AspectRatio				= AspectRatio_4_3;
	FMVAspectRatioSwitch	= FMV_AspectRatio_Switch_Off;
	Zoom					= 100;
	StretchY				= 100;
	OffsetX					= 0;
	OffsetY					= 0;
	IsMaximized				= false;
	IsFullscreen			= false;
	EnableVsyncWindowFlag	= false;

	IsToggleFullscreenOnDoubleClick = true;
}

void AppConfig::GSWindowOptions::SanityCheck()
{
	// Ensure Conformation of various options...
	if( (uint)AspectRatio >= (uint)AspectRatio_MaxCount )
		AspectRatio = AspectRatio_4_3;
}

void AppConfig::GSWindowOptions::LoadSave( IniInterface& ini )
{
	ScopedIniGroup path( ini, L"GSWindow" );

	IniEntry( CloseOnEsc );
	IniEntry( DefaultToFullscreen );
	IniEntry( AlwaysHideMouse );
	IniEntry( DisableResizeBorders );
	IniEntry( DisableScreenSaver );
	IniEntry( IsMaximized );
	IniEntry( IsFullscreen );
	IniEntry( EnableVsyncWindowFlag );

	IniEntry( IsToggleFullscreenOnDoubleClick );

	static const wxChar* AspectRatioNames[] =
	{
		L"Stretch",
		L"4:3",
		L"16:9",
		// WARNING: array must be NULL terminated to compute it size
		NULL
	};

	ini.EnumEntry( L"AspectRatio", AspectRatio, AspectRatioNames, AspectRatio );

	static const wxChar* FMVAspectRatioSwitchNames[] =
	{
		L"Off",
		L"4:3",
		L"16:9",
		// WARNING: array must be NULL terminated to compute it size
		NULL
	};
	ini.EnumEntry(L"FMVAspectRatioSwitch", FMVAspectRatioSwitch, FMVAspectRatioSwitchNames, FMVAspectRatioSwitch);

	IniEntry( Zoom );

	if( ini.IsLoading() ) SanityCheck();
}

// ----------------------------------------------------------------------------
AppConfig::FramerateOptions::FramerateOptions()
{
	NominalScalar			= 1.0;
	TurboScalar				= 2.0;
	SlomoScalar				= 0.50;

	SkipOnLimit				= false;
	SkipOnTurbo				= false;
}

void AppConfig::FramerateOptions::SanityCheck()
{
	// Ensure Conformation of various options...

	NominalScalar	.ConfineTo( 0.05, 10.0 );
	TurboScalar		.ConfineTo( 0.05, 10.0 );
	SlomoScalar		.ConfineTo( 0.05, 10.0 );
}

void AppConfig::FramerateOptions::LoadSave( IniInterface& ini )
{
	ScopedIniGroup path( ini, L"Framerate" );

	IniEntry( NominalScalar );
	IniEntry( TurboScalar );
	IniEntry( SlomoScalar );

	IniEntry( SkipOnLimit );
	IniEntry( SkipOnTurbo );
}

AppConfig::UiTemplateOptions::UiTemplateOptions()
{
	LimiterUnlimited	= L"Max";
	LimiterTurbo		= L"Turbo";
	LimiterSlowmo		= L"Slowmo";
	LimiterNormal		= L"Normal";
	OutputFrame			= L"Frame";
	OutputField			= L"Field";
	OutputProgressive	= L"Progressive";
	OutputInterlaced	= L"Interlaced";
	Paused				= L"<PAUSED> ";
	TitleTemplate		= L"Slot: ${slot} | Speed: ${speed} (${vfps}) | ${videomode} | Limiter: ${limiter} | ${gsdx} | ${omodei} | ${cpuusage}";
}

void AppConfig::UiTemplateOptions::LoadSave(IniInterface& ini)
{
	ScopedIniGroup path(ini, L"UiTemplates");

	IniEntry(LimiterUnlimited);
	IniEntry(LimiterTurbo);
	IniEntry(LimiterSlowmo);
	IniEntry(LimiterNormal);
	IniEntry(OutputFrame);
	IniEntry(OutputField);
	IniEntry(OutputProgressive);
	IniEntry(OutputInterlaced);
	IniEntry(Paused);
	IniEntry(TitleTemplate);
}

int AppConfig::GetMaxPresetIndex()
{
	return 5;
}

// Apply one of several (currently 6) configuration subsets.
// The scope of the subset which each preset controlls is hardcoded here.
// Use ignoreMTVU to avoid updating the MTVU field.
// Main purpose is for the preset enforcement at launch, to avoid overwriting a user's setting.
bool AppConfig::IsOkApplyPreset(int n, bool ignoreMTVU)
{
	if (n < 0 || n > GetMaxPresetIndex() )
	{
		Console.WriteLn("", n);
		log_cb(RETRO_LOG_INFO, "DEV Warning: ApplyPreset(%i): index out of range, Aborting.\n", n);
		return false;
	}

	log_cb(RETRO_LOG_INFO, "Applying preset n: %i\n", n);

	//Have some original and default values at hand to be used later.
	Pcsx2Config::GSOptions        original_GS = EmuOptions.GS;
	AppConfig::FramerateOptions	  original_Framerate = Framerate;
	Pcsx2Config::SpeedhackOptions original_SpeedHacks = EmuOptions.Speedhacks;
	AppConfig				default_AppConfig;
	Pcsx2Config				default_Pcsx2Config;

	//  NOTE:	Because the system currently only supports passing of an entire AppConfig to the GUI panels/menus to apply/reflect,
	//			the GUI entities should be aware of the settings which the presets control, such that when presets are used:
	//			1. The panels/entities should prevent manual modifications (by graying out) of settings which the presets control.
	//			2. The panels should not apply values which the presets don't control if the value is initiated by a preset.
	//			Currently controlled by the presets:
	//			- AppConfig:	Framerate (except turbo/slowmo factors), EnableSpeedHacks, EnableGameFixes.
	//			- EmuOptions:	Cpu, Gamefixes, SpeedHacks (except mtvu), EnablePatches, GS (except for FrameLimitEnable and VsyncEnable).
	//
	//			This essentially currently covers all the options on all the panels except for framelimiter which isn't
	//			controlled by the presets, and the entire GSWindow panel which also isn't controlled by presets
	//
	//			So, if changing the scope of the presets (making them affect more or less values), the relevant GUI entities
	//			should me modified to support it.


	//Force some settings as a (current) base for all presets.

	Framerate			= default_AppConfig.Framerate;
	Framerate.SlomoScalar = original_Framerate.SlomoScalar;
	Framerate.TurboScalar = original_Framerate.TurboScalar;

	EnableGameFixes		= false;

	EmuOptions.EnablePatches		= true;
	EmuOptions.GS					= default_Pcsx2Config.GS;
	EmuOptions.GS.FrameLimitEnable	= original_GS.FrameLimitEnable;	//Frame limiter is not modified by presets
	
	EmuOptions.Cpu					= default_Pcsx2Config.Cpu;
	EmuOptions.Gamefixes			= default_Pcsx2Config.Gamefixes;
	EmuOptions.Speedhacks			= default_Pcsx2Config.Speedhacks;
	EmuOptions.Speedhacks.bitset	= 0; //Turn off individual hacks to make it visually clear they're not used.
	EmuOptions.Speedhacks.vuThread	= original_SpeedHacks.vuThread;
	EnableSpeedHacks = true;
	// Actual application of current preset over the base settings which all presets use (mostly pcsx2's default values).

	bool isRateSet = false, isSkipSet = false, isMTVUSet = ignoreMTVU ? true : false; // used to prevent application of specific lower preset values on fallthrough.
	switch (n) // Settings will waterfall down to the Safe preset, then stop. So, Balanced and higher will inherit any settings through Safe.
	{
		case 5: // Mostly Harmful
			isRateSet ? 0 : (isRateSet = true, EmuOptions.Speedhacks.EECycleRate = 1); // +1 EE cyclerate
			isSkipSet ? 0 : (isSkipSet = true, EmuOptions.Speedhacks.EECycleSkip = 1); // +1 EE cycle skip
            // Fall through

		case 4: // Very Aggressive
			isRateSet ? 0 : (isRateSet = true, EmuOptions.Speedhacks.EECycleRate = -2); // -2 EE cyclerate
            // Fall through

		case 3: // Aggressive
			isRateSet ? 0 : (isRateSet = true, EmuOptions.Speedhacks.EECycleRate = -1); // -1 EE cyclerate
            // Fall through

		case 2: // Balanced
			isMTVUSet ? 0 : (isMTVUSet = true, EmuOptions.Speedhacks.vuThread = true); // Enable MTVU
            // Fall through

		case 1: // Safe (Default)
			EmuOptions.Speedhacks.IntcStat = true;
			EmuOptions.Speedhacks.WaitLoop = true;
			EmuOptions.Speedhacks.vuFlagHack = true;
			// If waterfalling from > Safe, break to avoid MTVU disable.
			if (n > 1) break;
            // Fall through
			
		case 0: // Safest
			isMTVUSet ? 0 : (isMTVUSet = true, EmuOptions.Speedhacks.vuThread = false); // Disable MTVU
			break;

		default:
			Console.WriteLn("Developer Warning: Preset #%d is not implemented. (--> Using application default).", n);
	}


	EnablePresets=true;
	PresetIndex=n;

	return true;
}

void AppConfig::ResetPresetSettingsToDefault() {
	EmuOptions.Speedhacks.EECycleRate = 0;
	EmuOptions.Speedhacks.EECycleSkip = 0;
	EmuOptions.Speedhacks.vuThread = false;
	EmuOptions.Speedhacks.IntcStat = true;
	EmuOptions.Speedhacks.WaitLoop = true;
	EmuOptions.Speedhacks.vuFlagHack = true;
	PresetIndex = 1;
	log_cb(RETRO_LOG_INFO, "Reset of speedhack preset to n:1 - Safe (default)\n");
}



wxFileConfig* OpenFileConfig( const wxString& filename )
{
	return new wxFileConfig( wxEmptyString, wxEmptyString, filename, wxEmptyString, wxCONFIG_USE_RELATIVE_PATH );
}

// Parameters:
//   overwrite - this option forces the current settings to overwrite any existing settings
//      that might be saved to the configured ini/settings folder.
//
// Notes:
//   The overwrite option applies to PCSX2 options only.  Plugin option behavior will depend
//   on the plugins.
//
void AppConfig_OnChangedSettingsFolder( bool overwrite )
{
	PathDefs::GetDocuments().Mkdir();
	GetSettingsFolder().Mkdir();


	const wxString iniFilename( GetUiSettingsFilename() );

	if( overwrite )
	{
		if( wxFileExists( iniFilename ) && !wxRemoveFile( iniFilename ) )
			throw Exception::AccessDenied(iniFilename)
				.SetBothMsgs(pxL("Failed to overwrite existing settings file; permission was denied."));

		const wxString vmIniFilename( GetVmSettingsFilename() );

		if( wxFileExists( vmIniFilename ) && !wxRemoveFile( vmIniFilename ) )
			throw Exception::AccessDenied(vmIniFilename)
				.SetBothMsgs(pxL("Failed to overwrite existing settings file; permission was denied."));
	}

	// Bind into wxConfigBase to allow wx to use our config internally, and delete whatever
	// comes out (cleans up prev config, if one).
	delete wxConfigBase::Set( OpenFileConfig( iniFilename ) );
	GetAppConfig()->SetRecordDefaults(true);

	if( !overwrite )
		AppLoadSettings();

	AppApplySettings();

	AppSaveSettings();//Make sure both ini files are created if needed.

}

// --------------------------------------------------------------------------------------
//  pxDudConfig
// --------------------------------------------------------------------------------------
// Used to handle config actions prior to the creation of the ini file (for example, the
// first time wizard).  Attempts to save ini settings are simply ignored through this
// class, which allows us to give the user a way to set everything up in the wizard, apply
// settings as usual, and only *save* something once the whole wizard is complete.
//
class pxDudConfig : public wxConfigBase
{
protected:
	wxString	m_empty;

public:
	virtual ~pxDudConfig() = default;

	virtual void SetPath(const wxString& ) {}
	virtual const wxString& GetPath() const { return m_empty; }

	virtual bool GetFirstGroup(wxString& , long& ) const { return false; }
	virtual bool GetNextGroup (wxString& , long& ) const { return false; }
	virtual bool GetFirstEntry(wxString& , long& ) const { return false; }
	virtual bool GetNextEntry (wxString& , long& ) const { return false; }
	virtual size_t GetNumberOfEntries(bool ) const  { return 0; }
	virtual size_t GetNumberOfGroups(bool ) const  { return 0; }

	virtual bool HasGroup(const wxString& ) const { return false; }
	virtual bool HasEntry(const wxString& ) const { return false; }

	virtual bool Flush(bool ) { return false; }

	virtual bool RenameEntry(const wxString&, const wxString& ) { return false; }

	virtual bool RenameGroup(const wxString&, const wxString& ) { return false; }

	virtual bool DeleteEntry(const wxString&, bool bDeleteGroupIfEmpty = true) { return false; }
	virtual bool DeleteGroup(const wxString& ) { return false; }
	virtual bool DeleteAll() { return false; }

protected:
	virtual bool DoReadString(const wxString& , wxString *) const  { return false; }
	virtual bool DoReadLong(const wxString& , long *) const  { return false; }

	virtual bool DoWriteString(const wxString& , const wxString& )  { return false; }
	virtual bool DoWriteLong(const wxString& , long )  { return false; }
};

static pxDudConfig _dud_config;

// --------------------------------------------------------------------------------------
//  AppIniSaver / AppIniLoader
// --------------------------------------------------------------------------------------
class AppIniSaver : public IniSaver
{
public:
	AppIniSaver();
	virtual ~AppIniSaver() = default;
};

class AppIniLoader : public IniLoader
{
public:
	AppIniLoader();
	virtual ~AppIniLoader() = default;
};

AppIniSaver::AppIniSaver()
	: IniSaver( (GetAppConfig() != NULL) ? *GetAppConfig() : _dud_config )
{
}

AppIniLoader::AppIniLoader()
	: IniLoader( (GetAppConfig() != NULL) ? *GetAppConfig() : _dud_config )
{
}

static void LoadUiSettings()
{
	AppIniLoader loader;
#ifndef __LIBRETRO__
	ConLog_LoadSaveSettings( loader );
	SysTraceLog_LoadSaveSettings( loader );
#endif
	g_Conf = std::make_unique<AppConfig>();
	g_Conf->LoadSave( loader );

	if( !wxFile::Exists( g_Conf->CurrentIso ) )
	{
		g_Conf->CurrentIso.clear();
	}

#if defined(_WIN32)
	if( !g_Conf->Folders.RunDisc.DirExists() )
	{
		g_Conf->Folders.RunDisc.Clear();
	}
#else
	if (!g_Conf->Folders.RunDisc.Exists())
	{
		g_Conf->Folders.RunDisc.Clear();
	}
#endif

	sApp.DispatchUiSettingsEvent( loader );
}

static void LoadVmSettings()
{
	// Load virtual machine options and apply some defaults overtop saved items, which
	// are regulated by the PCSX2 UI.

	std::unique_ptr<wxFileConfig> vmini( OpenFileConfig( GetVmSettingsFilename() ) );
	IniLoader vmloader( vmini.get() );
	g_Conf->EmuOptions.LoadSave( vmloader );
	g_Conf->EmuOptions.GS.LimitScalar = g_Conf->Framerate.NominalScalar;

	g_Conf->EnablePresets = option_value(BOOL_PCSX2_OPT_ENABLE_SPEEDHACKS, KeyOptionBool::return_type);
	g_Conf->PresetIndex = option_value(INT_PCSX2_OPT_SPEEDHACKS_PRESET, KeyOptionInt::return_type);
	

	if (g_Conf->EnablePresets) 
	{
		g_Conf->IsOkApplyPreset(g_Conf->PresetIndex, false);
	}
	else
	{
		g_Conf->ResetPresetSettingsToDefault();
	}

	sApp.DispatchVmSettingsEvent( vmloader );
}



void AppLoadSettings()
{
	if( wxGetApp().Rpc_TryInvoke(AppLoadSettings) ) return;

	LoadUiSettings();
	LoadVmSettings();
}

static void SaveUiSettings()
{	
	if( !wxFile::Exists( g_Conf->CurrentIso ) )
	{
		g_Conf->CurrentIso.clear();
	}

#if defined(_WIN32)
	if (!g_Conf->Folders.RunDisc.DirExists())
	{
		g_Conf->Folders.RunDisc.Clear();
	}
#else
	if (!g_Conf->Folders.RunDisc.Exists())
	{
		g_Conf->Folders.RunDisc.Clear();
	}
#endif
	AppIniSaver saver;
	g_Conf->LoadSave( saver );
#ifndef __LIBRETRO__
	ConLog_LoadSaveSettings( saver );
	SysTraceLog_LoadSaveSettings( saver );
#endif
	sApp.DispatchUiSettingsEvent( saver );
}

static void SaveVmSettings()
{
	std::unique_ptr<wxFileConfig> vmini( OpenFileConfig( GetVmSettingsFilename() ) );
	IniSaver vmsaver( vmini.get() );
	g_Conf->EmuOptions.LoadSave( vmsaver );

	sApp.DispatchVmSettingsEvent( vmsaver );
}

static void SaveRegSettings()
{
	std::unique_ptr<wxConfigBase> conf_install;

	if (InstallationMode == InstallMode_Portable) return;

	// sApp. macro cannot be use because you need the return value of OpenInstallSettingsFile method
	if( Pcsx2App* __app_ = (Pcsx2App*)wxApp::GetInstance() ) conf_install = std::unique_ptr<wxConfigBase>((*__app_).OpenInstallSettingsFile());
	conf_install->SetRecordDefaults(false);

	App_SaveInstallSettings( conf_install.get() );
}

void AppSaveSettings()
{
	// If multiple SaveSettings messages are requested, we want to ignore most of them.
	// Saving settings once when the GUI is idle should be fine. :)

	static std::atomic<bool> isPosted(false);

	if( !wxThread::IsMain() )
		return;

	//Console.WriteLn("Saving ini files...");

	SaveUiSettings();
	SaveVmSettings();
	SaveRegSettings(); // save register because of PluginsFolder change

	isPosted = false;
}


// Returns the current application configuration file.  This is preferred over using
// wxConfigBase::GetAppConfig(), since it defaults to *not* creating a config file
// automatically (which is typically highly undesired behavior in our system)
wxConfigBase* GetAppConfig()
{
	return wxConfigBase::Get( false );
}
